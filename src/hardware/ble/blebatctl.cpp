#include "config.h"
#include "blebatctl.h"
#include "bleupdater.h"
#include "gadgetbridge.h"

    #include <Arduino.h>


    class BleBattLevelUpdater : public BleUpdater<uint8_t> {
        public:
            BleBattLevelUpdater(BLECharacteristic *charac, time_t timeout) : BleUpdater(timeout), characteristic(charac) {}

            void set(uint8_t power) {
                characteristic->setValue(&power, 1);
            }
        
            bool notify( uint8_t level ) {
                bool retval = false;
                /**
                 * round level to 5 units
                 */
                level = level - ( level % 5 );
                /*
                 * Do not notify on same value
                 */
                if ( last_value == level || level <= 0 ) {
                    return( retval );
                }
                last_value = level;
                /*
                 * Send battery level via standard characteristic
                 */
                characteristic->notify();
                /*
                 * Send battery percent via BangleJS protocol
                 */
                bool ret = gadgetbridge_send_msg( "\r\n{t:\"status\", bat:%d}\r\n", level );

                return ret;
            }

            BLECharacteristic *characteristic;
    };

    class BleBattPowerUpdater : public BleUpdater<uint8_t> {
        public:
            BleBattPowerUpdater(BLECharacteristic *charac, time_t timeout) : BleUpdater(timeout), characteristic(charac) {}

            void set(uint8_t power) {
                characteristic->setValue(&power, 1);
            }

            bool notify(uint8_t power) {
                /*
                 * Do not notify on same value
                 */
                if ( last_value == power )
                    return( false );
                /*
                 * update power
                 */
                characteristic->notify();

                return( true );
            }
            BLECharacteristic *characteristic;
    };

    static bool blebatctl_pmu_event_cb( EventBits_t event, void *arg );
    static void blebatctl_update_battery( int32_t percent, bool charging, bool plug );
    static bool blebatctl_bluetooth_event_cb( EventBits_t event, void *arg );

    static BLECharacteristic *pBatteryLevelCharacteristic;
    static BLECharacteristic *pBatteryPowerStateCharacteristic;

    static BleBattLevelUpdater *blebatctl_level_updater;
    static BleBattPowerUpdater *blebatctl_power_updater;

    void blebatctl_setup( void ) {
        NimBLEServer *pServer = blectl_get_ble_server();
        NimBLEAdvertising *pAdvertising = blectl_get_ble_advertising();
        /*
         * Create battery service
         */
        NimBLEService *pBatteryService = pServer->createService( BATTERY_SERVICE_UUID );
        /*
         * Create a BLE battery service, batttery level Characteristic - 
         */
        pBatteryLevelCharacteristic = pBatteryService->createCharacteristic( BATTERY_LEVEL_CHARACTERISTIC_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY );
        pBatteryPowerStateCharacteristic = pBatteryService->createCharacteristic( BATTERY_POWER_STATE_CHARACTERISTIC_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY );
        pBatteryService->start();
        /*
         * Start advertising battery service
         */
        pAdvertising->addServiceUUID( pBatteryService->getUUID() );

        blebatctl_level_updater = new BleBattLevelUpdater( pBatteryLevelCharacteristic, 60 * 5 );
        blebatctl_power_updater = new BleBattPowerUpdater( pBatteryPowerStateCharacteristic, 60 * 6 );

        //pmu_register_cb( PMUCTL_STATUS, blebatctl_pmu_event_cb, "ble battery" );
        gadgetbridge_register_cb( GADGETBRIDGE_CONNECT, blebatctl_bluetooth_event_cb, "ble battery" );
    }

    static bool blebatctl_pmu_event_cb( EventBits_t event, void *arg ) {
        bool retval = false;

        switch( event ) {
            /*case PMUCTL_STATUS:
                bool charging = *(bool*)arg & PMUCTL_STATUS_CHARGING;
                bool plug = *(bool*)arg & PMUCTL_STATUS_PLUG;
                int32_t percent = *(int32_t*)arg & PMUCTL_STATUS_PERCENT;
                blebatctl_update_battery( percent, charging, plug );
                retval = true;
                break;
                */
        }
        return( retval );
    }

    static void blebatctl_update_battery( int32_t percent, bool charging, bool plug ) {
        /*
         * update battery level
         */
        uint8_t level = (uint8_t)percent;
        if ( level > 100 ) {
            level = 100;
        }
        blebatctl_level_updater->update( level );
        /*
         * Send powerstate via standard caracteristic
         */
        uint8_t batteryPowerState = BATTERY_POWER_STATE_BATTERY_PRESENT
                                    | ( plug ? BATTERY_POWER_STATE_DISCHARGE_NOT_DISCHARING : BATTERY_POWER_STATE_DISCHARGE_DISCHARING )
                                    | ( charging ? BATTERY_POWER_STATE_CHARGE_CHARING : BATTERY_POWER_STATE_CHARGE_NOT_CHARING )
                                    | ( percent > 10 ? BATTERY_POWER_STATE_LEVEL_GOOD : BATTERY_POWER_STATE_LEVEL_CRITICALLY_LOW );
        blebatctl_power_updater->update( batteryPowerState );
    }

    static bool blebatctl_bluetooth_event_cb(EventBits_t event, void *arg) {
        bool retval = false;

        switch( event ) {
            case GADGETBRIDGE_CONNECT:        
                    /*
                     * Try to refresh values on (re)connect
                     */
                    //bool charging = pmu_is_charging();
                    //bool plug = pmu_is_vbus_plug();
                    //int32_t percent = pmu_get_battery_percent();
                    bool charging = false;
                    bool plug = false;
                    int32_t percent = 76;

                    blebatctl_update_battery( percent, charging, plug );
                    retval = true;
                    break;
        }

        return( retval );
    }
    