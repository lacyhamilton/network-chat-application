#include "../properties.h"

int main(int argc, const char * argv[]) 
{

    char* properties_file = "properties.txt";
    Properties* properties;
    char* key = "CHAT_PORT";
    char* chat_port;
    
    properties = property_read_properties(properties_file);
    chat_port = property_get_property(properties, key);
    key = "SERVER_PORT";
    char *server_port = property_get_property(properties, key);
    key = "SERVER_IP";
    char *server_ip= property_get_property(properties, key);
    key = "CHAT_IP";
    char *chat_ip= property_get_property(properties, key);
    

    return EXIT_SUCCESS;
}
