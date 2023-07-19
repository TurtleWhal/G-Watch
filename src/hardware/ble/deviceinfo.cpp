#include "config.h"
#include "deviceinfo.h"
#include "hardware/blectl.h"

#define __FIRMWARE__ "20230717"

void deviceinfo_setup(void)
{
    NimBLEServer *pServer = blectl_get_ble_server();
    NimBLEAdvertising *pAdvertising = blectl_get_ble_advertising();
    /*
     * Create device information service
     */
    NimBLEService *pDeviceInformationService = pServer->createService(NimBLEUUID(DEVICE_INFORMATION_SERVICE_UUID));
    NimBLECharacteristic *pManufacturerNameStringCharacteristic = pDeviceInformationService->createCharacteristic(MANUFACTURER_NAME_STRING_CHARACTERISTIC_UUID, NIMBLE_PROPERTY::READ);
    pManufacturerNameStringCharacteristic->setValue("Lily Go");
    NimBLECharacteristic *pFirmwareRevisionStringCharacteristic = pDeviceInformationService->createCharacteristic(FIRMWARE_REVISION_STRING_CHARACTERISTIC_UUID, NIMBLE_PROPERTY::READ);
    pFirmwareRevisionStringCharacteristic->setValue(__FIRMWARE__);
    pDeviceInformationService->start();
    pAdvertising->addServiceUUID(pDeviceInformationService->getUUID());
}