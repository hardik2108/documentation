#ifndef TT_CGI_H
#define TT_CGI_H 1

/* show errors if dependencies are not included */

#if !LWIP_HTTPD_CUSTOM_FILES
#error This needs LWIP_HTTPD_CUSTOM_FILES
#endif

#if !LWIP_HTTPD_DYNAMIC_HEADERS
#error This needs LWIP_HTTPD_DYNAMIC_HEADERS
#endif

#if !LWIP_HTTPD_CGI
#error This needs LWIP_HTTPD_CGI
#endif

#define PROJECT_ENDPOINT "/project.html"
#define API_SENSOR_DATA "/api/sensor-data"
#define GET_DHT22_DATA_ENDPOINT "/dht22.json"



/* initialization functions */
void custom_files_init(void);
void cgi_init(void);

#endif
