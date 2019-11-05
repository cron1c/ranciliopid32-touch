void fmtDouble(double val, byte precision, char *buf, unsigned bufLen = 0xffff);
unsigned fmtUnsigned(unsigned long val, char *buf, unsigned bufLen = 0xffff, byte width = 0);

//
// Produce a formatted string in a buffer corresponding to the value provided.
// If the 'width' parameter is non-zero, the value will be padded with leading
// zeroes to achieve the specified width.  The number of characters added to
// the buffer (not including the null termination) is returned.
//
unsigned
fmtUnsigned(unsigned long val, char *buf, unsigned bufLen, byte width)
{
  if (!buf || !bufLen)
    return (0);

  // produce the digit string (backwards in the digit buffer)
  char dbuf[10];
  unsigned idx = 0;
  while (idx < sizeof(dbuf))
  {
    dbuf[idx++] = (val % 10) + '0';
    if ((val /= 10) == 0)
      break;
  }

  // copy the optional leading zeroes and digits to the target buffer
  unsigned len = 0;
  byte padding = (width > idx) ? width - idx : 0;
  char c = '0';
  while ((--bufLen > 0) && (idx || padding))
  {
    if (padding)
      padding--;
    else
      c = dbuf[--idx];
    *buf++ = c;
    len++;
  }

  // add the null termination
  *buf = '\0';
  return (len);
}

//
// Format a floating point value with number of decimal places.
// The 'precision' parameter is a number from 0 to 6 indicating the desired decimal places.
// The 'buf' parameter points to a buffer to receive the formatted string.  This must be
// sufficiently large to contain the resulting string.  The buffer's length may be
// optionally specified.  If it is given, the maximum length of the generated string
// will be one less than the specified value.
//
// example: fmtDouble(3.1415, 2, buf); // produces 3.14 (two decimal places)
//
void
fmtDouble(double val, byte precision, char *buf, unsigned bufLen)
{
  if (!buf || !bufLen)
    return;

  // limit the precision to the maximum allowed value
  const byte maxPrecision = 6;
  if (precision > maxPrecision)
    precision = maxPrecision;

  if (--bufLen > 0)
  {
    // check for a negative value
    if (val < 0.0)
    {
      val = -val;
      *buf = '-';
      bufLen--;
    }

    // compute the rounding factor and fractional multiplier
    double roundingFactor = 0.5;
    unsigned long mult = 1;
    for (byte i = 0; i < precision; i++)
    {
      roundingFactor /= 10.0;
      mult *= 10;
    }

    if (bufLen > 0)
    {
      // apply the rounding factor
      val += roundingFactor;

      // add the integral portion to the buffer
      unsigned len = fmtUnsigned((unsigned long)val, buf, bufLen);
      buf += len;
      bufLen -= len;
    }

    // handle the fractional portion
    if ((precision > 0) && (bufLen > 0))
    {
      *buf++ = '.';
      if (--bufLen > 0)
        buf += fmtUnsigned((unsigned long)((val - (unsigned long)val) * mult), buf, bufLen, precision);
    }
  }

  // null-terminate the string
  *buf = '\0';
}



#include <string.h>
#include <FreeRTOS.h>
#include <freertos/task.h>
#include <cJSON.h>
#include <esp_system.h>
#include <esp_log.h>
#include <esp_http_client.h>
#include <esp_https_ota.h>
#include <esp_ota_ops.h>
#define FIRMWARE_VERSION  1.1
#define UPDATE_JSON_URL   "http://cbin.de/esp32/firmware.json"
#define BLINK_GPIO      99

// server certificates
extern const char server_cert_pem_start[] asm("_binary_certs_pem_start");
extern const char server_cert_pem_end[] asm("_binary_certs_pem_end");
const char* cert = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIEkjCCA3qgAwIBAgIQCgFBQgAAAVOFc2oLheynCDANBgkqhkiG9w0BAQsFADA/\n" \
"MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
"DkRTVCBSb290IENBIFgzMB4XDTE2MDMxNzE2NDA0NloXDTIxMDMxNzE2NDA0Nlow\n" \
"SjELMAkGA1UEBhMCVVMxFjAUBgNVBAoTDUxldCdzIEVuY3J5cHQxIzAhBgNVBAMT\n" \
"GkxldCdzIEVuY3J5cHQgQXV0aG9yaXR5IFgzMIIBIjANBgkqhkiG9w0BAQEFAAOC\n" \
"AQ8AMIIBCgKCAQEAnNMM8FrlLke3cl03g7NoYzDq1zUmGSXhvb418XCSL7e4S0EF\n" \
"q6meNQhY7LEqxGiHC6PjdeTm86dicbp5gWAf15Gan/PQeGdxyGkOlZHP/uaZ6WA8\n" \
"SMx+yk13EiSdRxta67nsHjcAHJyse6cF6s5K671B5TaYucv9bTyWaN8jKkKQDIZ0\n" \
"Z8h/pZq4UmEUEz9l6YKHy9v6Dlb2honzhT+Xhq+w3Brvaw2VFn3EK6BlspkENnWA\n" \
"a6xK8xuQSXgvopZPKiAlKQTGdMDQMc2PMTiVFrqoM7hD8bEfwzB/onkxEz0tNvjj\n" \
"/PIzark5McWvxI0NHWQWM6r6hCm21AvA2H3DkwIDAQABo4IBfTCCAXkwEgYDVR0T\n" \
"AQH/BAgwBgEB/wIBADAOBgNVHQ8BAf8EBAMCAYYwfwYIKwYBBQUHAQEEczBxMDIG\n" \
"CCsGAQUFBzABhiZodHRwOi8vaXNyZy50cnVzdGlkLm9jc3AuaWRlbnRydXN0LmNv\n" \
"bTA7BggrBgEFBQcwAoYvaHR0cDovL2FwcHMuaWRlbnRydXN0LmNvbS9yb290cy9k\n" \
"c3Ryb290Y2F4My5wN2MwHwYDVR0jBBgwFoAUxKexpHsscfrb4UuQdf/EFWCFiRAw\n" \
"VAYDVR0gBE0wSzAIBgZngQwBAgEwPwYLKwYBBAGC3xMBAQEwMDAuBggrBgEFBQcC\n" \
"ARYiaHR0cDovL2Nwcy5yb290LXgxLmxldHNlbmNyeXB0Lm9yZzA8BgNVHR8ENTAz\n" \
"MDGgL6AthitodHRwOi8vY3JsLmlkZW50cnVzdC5jb20vRFNUUk9PVENBWDNDUkwu\n" \
"Y3JsMB0GA1UdDgQWBBSoSmpjBH3duubRObemRWXv86jsoTANBgkqhkiG9w0BAQsF\n" \
"AAOCAQEA3TPXEfNjWDjdGBX7CVW+dla5cEilaUcne8IkCJLxWh9KEik3JHRRHGJo\n" \
"uM2VcGfl96S8TihRzZvoroed6ti6WqEBmtzw3Wodatg+VyOeph4EYpr/1wXKtx8/\n" \
"wApIvJSwtmVi4MFU5aMqrSDE6ea73Mj2tcMyo5jMd6jmeWUHK8so/joWUoHOUgwu\n" \
"X4Po1QYz+3dszkDqMp4fklxBwXRsW10KXzPMTZ+sOPAveyxindmjkW8lGy+QsRlG\n" \
"PfZ+G6Z6h7mjem0Y+iWlkYcV4PIWL1iwBi8saCbGS5jN2p8M+X+Q7UNKEkROb3N6\n" \
"KOqkqm57TH2H3eDJAkSnh6/DNFu0Qg==\n" \
"-----END CERTIFICATE-----\n";
// receive buffer
char rcv_buffer[200];


// esp_http_client event handler
esp_err_t _http_event_handler(esp_http_client_event_t *evt) {

  switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
      break;
    case HTTP_EVENT_ON_CONNECTED:
      break;
    case HTTP_EVENT_HEADER_SENT:
      break;
    case HTTP_EVENT_ON_HEADER:
      break;
    case HTTP_EVENT_ON_DATA:
      if (!esp_http_client_is_chunked_response(evt->client)) {
        strncpy(rcv_buffer, (char*)evt->data, evt->data_len);
      }
      break;
    case HTTP_EVENT_ON_FINISH:
      break;
    case HTTP_EVENT_DISCONNECTED:
      break;
  }
  return ESP_OK;
}

// Check update task
// downloads every 30sec the json file with the latest firmware
void check_update_task(void *pvParameter) {

  while (1) {

    Serial.printf("Looking for a new firmware...\n");

    // configure the esp_http_client
    esp_http_client_config_t config {};
    config.url = UPDATE_JSON_URL;
    config.event_handler = _http_event_handler;

    esp_http_client_handle_t client = esp_http_client_init(&config);

    // downloading the json file
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {

      // parse the json file
      cJSON *json = cJSON_Parse(rcv_buffer);
      if (json == NULL) Serial.printf("downloaded file is not a valid json, aborting...\n");
      else {
        cJSON *version = cJSON_GetObjectItemCaseSensitive(json, "version");
        cJSON *file = cJSON_GetObjectItemCaseSensitive(json, "file");

        // check the version
        if (!cJSON_IsNumber(version)) Serial.printf("unable to read new version, aborting...\n");
        else {

          double new_version = version->valuedouble;
          if (new_version > FIRMWARE_VERSION) {

            Serial.printf("current firmware version (%.1f) is lower than the available one (%.1f), upgrading...\n", FIRMWARE_VERSION, new_version);
            if (cJSON_IsString(file) && (file->valuestring != NULL)) {
              Serial.printf("downloading and installing new firmware (%s)...\n", file->valuestring);


              esp_http_client_config_t ota_client_config;
              ota_client_config.url = file->valuestring;
              ota_client_config.cert_pem = cert;
              esp_err_t ret = esp_https_ota(&ota_client_config);
              if (ret == ESP_OK) {
                Serial.printf("OTA OK, restarting...\n");
                esp_restart();
              } else {
                Serial.printf("OTA failed...\n");
              }
            }
            else Serial.printf("unable to read the new file name, aborting...\n");
          }
          else Serial.printf("current firmware version (%.1f) is greater or equal to the available one (%.1f), nothing to do...\n", FIRMWARE_VERSION, new_version);
        }
      }
    }
    else Serial.printf("unable to download the json file, aborting...\n");

    // cleanup
    esp_http_client_cleanup(client);

    Serial.printf("\n");
    vTaskDelay(600000 / portTICK_PERIOD_MS);
  }
}
