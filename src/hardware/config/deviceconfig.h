#ifndef _DEVICE_CONFIG_H
    #define _DEVICE_CONFIG_H

    #include "config.h"
    #include "utils/basejsonconfig.h"

    #define DEVICE_JSON_CONFIG_FILE    "/device.json" /** @brief defines json config file name */
    
    /**
     * @brief display config structure
     */
    class device_config_t : public BaseJsonConfig {
        public:
            device_config_t();
            //char device_name[64] = HARDWARE_NAME;      /** @brief device name */
            char device_name[64] = "Twatch2021";

        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 1000; }
    };
#endif // _DEVICE_CONFIG_H