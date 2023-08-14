#define CAMERA_MODEL_AI_THINKER
#include <Arduino.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>
#include <FS.h>
#include <EloquentArduino.h>
#include <eloquentarduino/io/serial_print.h>
#include <eloquentarduino/vision/camera/ESP32Camera.h>
#include <eloquentarduino/vision/io/writers/JpegWriter.h>
#include <eloquentarduino/vision/io/decoders/Red565RandomAccessDecoder.h>
#include <eloquentarduino/vision/processing/downscaling/Downscaler.h>
#include <eloquentarduino/vision/processing/MotionDetector.h>


#define FRAME_SIZE FRAMESIZE_VGA
#define PIXFORMAT PIXFORMAT_RGB565
#define W 640
#define H 480
#define w 80
#define h 60
#define DIFF_THRESHOLD 10 // set how much a pixel value should differ to be considered as a change
#define MOTION_THRESHOLD 0.10 // set how many pixels (in percent) should change to be considered as motion

// delete the second definition if you want to turn on code benchmarking
#define timeit(label, code) { uint32_t start = millis(); code; uint32_t duration = millis() - start; eloquent::io::print_all("It took ", duration, " millis for ", label); }
#define timeit(label, code) code;

using namespace Eloquent::Vision;
int i = 0;
camera_fb_t *frame;
Camera::ESP32Camera camera(PIXFORMAT);
uint8_t downscaled[w * h];
//IO::Decoders::GrayscaleRandomAccessDecoder decoder;
IO::Decoders::Red565RandomAccessDecoder decoder;
Processing::Downscaling::Center < W / w, H / h > strategy;
Processing::Downscaling::Downscaler<W, H, w, h> downscaler(&decoder, &strategy);
Processing::MotionDetector<w, h> motion;
IO::Writers::JpegWriter<W, H> jpegWriter;

int img_no=0;

const char* ssid = "";
const char* password = "";

uint8_t connection_state = 0;
uint16_t reconnect_interval = 10000;

#define SMTP_HOST "smtp.gmail.com"

/** The smtp port e.g.
   25  or esp_mail_smtp_port_25
   465 or esp_mail_smtp_port_465
   587 or esp_mail_smtp_port_587
*/
#define SMTP_PORT esp_mail_smtp_port_587

/* The log in credentials */
#define AUTHOR_EMAIL ""
#define AUTHOR_PASSWORD ""

/* The SMTP Session object used for Email sending */
SMTPSession smtp;

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status);

void capture();

void setup() {
  Serial.begin(115200);
  //SPIFFS.begin(true);
  delay(1000);
  Serial.println("Starting.....");
  Serial.print("Connecting to ");

  WiFi.begin(ssid, password);
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print(".");
  }
  Serial.println("Connection: ESTABLISHED");
  Serial.print("Got IP address: ");
  Serial.println(WiFi.localIP());

  
  delay(1000);
  camera.begin(FRAME_SIZE);
  motion.setDiffThreshold(DIFF_THRESHOLD);

  motion.setMotionThreshold(MOTION_THRESHOLD);
  // prevent consecutive triggers
  motion.setDebounceFrames(5);

  
  delay(1000);
  for (int i = 0; i < 5; i++) { //taking 5 dummy shots to make the camera image stable after boot
    capture();
  }

}

void capture() {
  timeit("capture frame", frame = camera.capture());

  // scale image from size H * W to size h * w
  timeit("downscale", downscaler.downscale(frame->buf, downscaled));

  // detect motion on the downscaled image
  timeit("motion detection", motion.detect(downscaled));
}


void loop() {
  capture();
  eloquent::io::print_all(motion.changes(), " pixels changed");

  if (motion.triggered()) {
    Serial.println("Motion detected");
    sendmail();
  }
}


void sendmail() {

  uint8_t * jpg_buf;
  size_t jpg_size;
  
    // Convert the RAW image into JPG
    // The parameter "31" is the JPG quality. Higher is better. But keep in mind that heap memory requirement also increases 
  fmt2jpg(frame->buf, frame->len, frame->width, frame->height, frame->format, 31, &jpg_buf, &jpg_size); // converting rgb565 frame to JPEG frame, frame stored in jpg_buf
  printf("Converted JPG size: %d bytes \n", jpg_size);
  
  ESP_MAIL_PRINTF("Free Heap: %d\n", MailClient.getFreeHeap());
  smtp.debug(1);

  /* Set the callback function to get the sending results */
  smtp.callback(smtpCallback);

  /* Declare the session config data */
  ESP_Mail_Session session;

  /* Set the session config */
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;

  /* Declare the message class */
  SMTP_Message message;

  /* Enable the chunked data transfer with pipelining for large message if server supported */
  message.enable.chunking = true;

  message.sender.email = AUTHOR_EMAIL;

  message.subject = F("Motion detected!");
  message.addRecipient(F("recipient_name"), F("recipient@gmail.com"));
  String textMsg = "Bed room";
  message.text.content = textMsg;
  message.text.charSet = F("us-ascii");
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_normal;
  /* The attachment data item */
  SMTP_Attachment att;

  /** Set the inline image info e.g.
     file name, MIME type and transfer encoding
  */
  att.descr.filename = F("capture.jpg");
  att.descr.mime = F("image/jpg");

  att.blob.data = jpg_buf;
  att.blob.size = jpg_size;

  att.descr.transfer_encoding = Content_Transfer_Encoding::enc_base64;
  message.addAttachment(att);

  ESP_MAIL_PRINTF("Free Heap: %d\n", MailClient.getFreeHeap());
  Serial.println("attached");

  if (!smtp.connect(&session))
    return;
  
  /* Start sending the Email and close the session */
  if (!MailClient.sendMail(&smtp, &message, true))
    digitalWrite(4,1);
    Serial.println("Error sending Email, " + smtp.errorReason());

  //to clear sending result log
  //smtp.sendingResult.clear();

  ESP_MAIL_PRINTF("Free Heap: %d\n", MailClient.getFreeHeap());


}

void smtpCallback(SMTP_Status status)
{
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success())
  {
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++)
    {
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients);
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject);
    }
    Serial.println("----------------\n");
  }
}