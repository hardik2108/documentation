/*
 * Implementation of CGI handlers and adoption of generated JSON to sensors use case.
 */

#include "lwip/apps/httpd.h"
#include "lwip/opt.h"

#include "lwip/apps/fs.h"
#include "lwip/def.h"
#include "lwip/mem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bl_gpio.h>

#include "cJSON.h"

#include "cgi.h"

#include "dht22.h"
#include "relay.h"

/* opening (creating) the in real-time created file (page) */
int fs_open_custom(struct fs_file *file, const char *name)
{
  cJSON *json_response = NULL;
  char *response = NULL;
    if (!strcmp(name, PROJECT_ENDPOINT))
    {
        /* Show sensor data in a styled HTML page */
        response = (char *)calloc(2048, sizeof(char));

        strcat(response, "<!DOCTYPE html><html><head>");
        strcat(response, "<meta charset='UTF-8'>");
        strcat(response, "<title>Air Quality & Temperature</title>");
        strcat(response, "<style>");
        strcat(response, "body { font-family: Arial, sans-serif; margin: 0; padding: 0; background-color: #f5f5f5; }");
        strcat(response, ".header { background-color:rgb(70, 205, 39); color: white; padding: 20px; text-align: center; }");
        strcat(response, "h1 { margin: 0; }");
        strcat(response, ".logo { width: 50px; vertical-align: middle; margin-right: 10px; }");
        strcat(response, ".content { padding: 20px; }");
        strcat(response, ".sensor-box { background: white; border: 1px solid #ddd; padding: 20px; margin: 10px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }");
        strcat(response, ".value { font-size: 24px; margin: 10px 0; }");
        strcat(response, ".unit { font-size: 14px; color: #666; }");
        strcat(response, "</style>");

        strcat(response, "<script>");
        strcat(response, "async function updateSensorData() {");
        strcat(response, "  try {");
        strcat(response, "    const response = await fetch('/api/sensor-data');");
        strcat(response, "    const data = await response.json();");
        strcat(response, "    updateUI(data);");
        strcat(response, "  } catch (error) {");
        strcat(response, "    console.error('Error fetching sensor data:', error);");
        strcat(response, "  }");
        strcat(response, "}");

        strcat(response, "function updateUI(data) {");
        strcat(response, "  document.getElementById('temperature').textContent = data.temperature;");
        strcat(response, "  document.getElementById('humidity').textContent = data.humidity;");
        strcat(response, "}");
        strcat(response, "setInterval(updateSensorData, 2000);");
        strcat(response, "updateSensorData();");
        strcat(response, "</script>");

        strcat(response, "</head><body>");
        strcat(response, "<div class='header'>");
        strcat(response, "<h1>Comfort Sphere</h1>");
        strcat(response, "</div>");

        strcat(response, "<div class='content'>");

        strcat(response, "<div id='temp-box' class='sensor-box'>");
        strcat(response, "<h2>Temperature</h2>");
        char temp_str[100];
        snprintf(temp_str, sizeof(temp_str), "<div class='value'><span id='temperature'>%d.%d</span><span class='unit'>°C</span></div>", temperature / 10, temperature % 10);
        strcat(response, temp_str);
        strcat(response, "</div>");

        strcat(response, "<div id='humidity-box' class='sensor-box'>");
        strcat(response, "<h2>Humidity</h2>");
        char humid_str[100];
        snprintf(humid_str, sizeof(humid_str), "<div class='value'><span id='humidity'>%d.%d</span><span class='unit'>%%</span></div>", humidity / 10, humidity % 10);
        strcat(response, humid_str);
        strcat(response, "</div>");

        strcat(response, "</div>");
        strcat(response, "</body></html>");
    }
  else if (!strcmp(name, API_SENSOR_DATA))
  {
      // Create JSON response for API endpoint
      cJSON *json_response = cJSON_CreateObject();
      
      // Format temperature and humidity as floats
      float temp = temperature / 10.0f;
      float hum = humidity / 10.0f;
      cJSON_AddNumberToObject(json_response, "temperature", temp);
      cJSON_AddNumberToObject(json_response, "humidity", hum);

      // Add device statuses
      int light_status = bl_gpio_input_get_value(RELAY_LIGHT_PIN);
      int heater_status = bl_gpio_input_get_value(RELAY_HEATER_PIN);
      int humidifier_status = bl_gpio_input_get_value(RELAY_HUMIDIFIER_PIN);
      
      cJSON_AddBoolToObject(json_response, "light", !light_status);
      cJSON_AddBoolToObject(json_response, "heater", !heater_status);
      cJSON_AddBoolToObject(json_response, "humidifier", !humidifier_status);

      response = cJSON_PrintUnformatted(json_response);
      cJSON_Delete(json_response);
  }
  else if (!strcmp(name, GET_DHT22_DATA_ENDPOINT))
  {
    json_response = cJSON_CreateObject();

    // Create formatted strings for temperature and humidity
    char temperature_str[10];
    char humidity_str[10];
    snprintf(temperature_str, sizeof(temperature_str), "%d.%d°C", temperature / 10, temperature % 10);
    snprintf(humidity_str, sizeof(humidity_str), "%d.%d%%", humidity / 10, humidity % 10);

    // Add formatted strings to JSON object
    cJSON_AddStringToObject(json_response, "Temperature", temperature_str);
    cJSON_AddStringToObject(json_response, "Humidity", humidity_str);

    response = cJSON_PrintUnformatted(json_response);
  }
  else
  {
    /* send null if unknown URI */
    return 0;
  }

  int response_size = strlen(response);

  /* allocate memory */
  memset(file, 0, sizeof(struct fs_file));
  file->pextension = mem_malloc(response_size);
  if (file->pextension != NULL)
  {
    /* copy json to file handler */
    memcpy(file->pextension, response, response_size);
    file->data = (const char *)file->pextension;
    file->len = response_size;
    file->index = file->len;
    /* allow persisting connections */
    file->flags = FS_FILE_FLAGS_HEADER_PERSISTENT;
  }

  /* free no longer needed memory */
  if (json_response != NULL)
  {
    // delete json structure
    cJSON_Delete(json_response);
  }
  if (response != NULL)
  {
    free(response);
  }

  /* return whether data was sent */
  if (file->pextension != NULL)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/* closing the custom file (free the memory) */
void fs_close_custom(struct fs_file *file)
{
  if (file && file->pextension)
  {
    mem_free(file->pextension);
    file->pextension = NULL;
  }
}

/* reading the custom file (nothing has to be done here, but function must be defined */
int fs_read_custom(struct fs_file *file, char *buffer, int count)
{
  LWIP_UNUSED_ARG(file);
  LWIP_UNUSED_ARG(buffer);
  LWIP_UNUSED_ARG(count);
  return FS_READ_EOF;
}

/* initialization functions */
void custom_files_init(void)
{
  printf("Initializing module for generating JSON output\r\n");
  /* Nothing to do as of now, should be initialized automatically */
}

void cgi_init(void)
{
  printf("Initializing module for CGI\r\n");
  //http_set_cgi_handlers(cgi_handlers, LWIP_ARRAYSIZE(cgi_handlers));
}
