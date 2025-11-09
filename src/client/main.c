#include "../properties.h"

int main(int argc, const char * argv[]) 
{

    char* properties_file = "properties.txt";
    Properties* properties;
    char* key = "PORT";
    char* value;
    
    properties = property_read_properties(properties_file);
    value = property_get_property(properties, key);
    
    printf("\nValue for %s: %s\n", key, value);

    return EXIT_SUCCESS;
}
