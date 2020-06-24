/*
 * BLEServer.h
 *
 *  Created on: Apr 16, 2017
 *      Author: kolban
 */

#ifndef COMPONENTS_CPP_UTILS_BLESERVER_H_
#define COMPONENTS_CPP_UTILS_BLESERVER_H_
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)
#include <esp_gatts_api.h>

#include <string>
#include <string.h>

#include "BLEUUID.h"
#include "BLEAdvertising.h"
#include "BLECharacteristic.h"
#include "BLEService.h"
#include "BLESecurity.h"
#include "FreeRTOS.h"

class BLEServerCallbacks;


/**
 * @brief A data structure that manages the %BLE servers owned by a BLE server.
 */
class BLEServiceMap {
public:
	BLEService* getByHandle(uint16_t handle);
	BLEService* getByUUID(const char* uuid);	
	BLEService* getByUUID(BLEUUID uuid);
	void        handleGATTServerEvent(
		esp_gatts_cb_event_t      event,
		esp_gatt_if_t             gatts_if,
		esp_ble_gatts_cb_param_t* param);
	void        setByHandle(uint16_t handle, BLEService* service);
	void        setByUUID(const char* uuid, BLEService* service);
	void        setByUUID(BLEUUID uuid, BLEService* service);
	std::string toString();
	BLEService* getFirst();
	BLEService* getNext();
	void 		removeService(BLEService *service);

private:
	std::map<uint16_t, BLEService*>    m_handleMap;
	std::map<BLEService*, std::string> m_uuidMap;
	std::map<BLEService*, std::string>::iterator m_iterator;
};


/**
 * @brief The model of a %BLE server.
 */
class BLEServer {
public:
	~BLEServer() ;
	uint32_t        getConnectedCount();
	BLEService*     createService(const char* uuid);	
	BLEService*     createService(BLEUUID uuid, uint32_t numHandles=15, uint8_t inst_id=0);
	BLEAdvertising* getAdvertising();
	void            setCallbacks(BLEServerCallbacks* pCallbacks);
	void            startAdvertising();
	void 			removeService(BLEService *service);
    void            updateConnParams(esp_bd_addr_t remote_bda, uint16_t minInterval, uint16_t maxInterval, uint16_t latency, uint16_t timeout);
	void 			disconnectClient(void);

private:
	BLEServer();
	friend class BLEService;
	friend class BLECharacteristic;
	friend class BLEDevice;
	esp_ble_adv_data_t  m_adv_data;
	uint16_t            m_appId;
	BLEAdvertising      m_bleAdvertising;
	uint16_t			m_connId;
	uint32_t            m_connectedCount;
	uint16_t            m_gatts_if;

	FreeRTOS::Semaphore m_semaphoreRegisterAppEvt 	= FreeRTOS::Semaphore("RegisterAppEvt");
	FreeRTOS::Semaphore m_semaphoreUnregisterAppEvt = FreeRTOS::Semaphore("UnregisterAppEvt");
	FreeRTOS::Semaphore m_semaphoreCreateEvt 		= FreeRTOS::Semaphore("CreateEvt");
	FreeRTOS::Semaphore m_semaphoreOpenEvt   		= FreeRTOS::Semaphore("OpenEvt");
	FreeRTOS::Semaphore m_semaphoreCloseEvt   		= FreeRTOS::Semaphore("CloseEvt");

	BLEServiceMap       m_serviceMap;
	BLEServerCallbacks* m_pServerCallbacks;

	void            createApp(uint16_t appId);
	void            deleteApp(void);
	uint16_t        getConnId();
	uint16_t        getGattsIf();
	void            handleGAPEvent(esp_gap_ble_cb_event_t event,	esp_ble_gap_cb_param_t *param);
	void            handleGATTServerEvent(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
	void            registerApp();
	void            unregisterApp(uint16_t);
}; // BLEServer


/**
 * @brief Callbacks associated with the operation of a %BLE server.
 */
class BLEServerCallbacks {
public:
	virtual ~BLEServerCallbacks() {};
	/**
	 * @brief Handle a new client connection.
	 *
	 * When a new client connects, we are invoked.
	 *
	 * @param [in] pServer A reference to the %BLE server that received the client connection.
	 */
	virtual void onConnect(BLEServer* pServer);

	/**
	 * @brief Handle a new client connection
	 *
	 * @param [in] pServer A reference to the %BLE server that received the client connection.
	 * @param [in] param A reference to the union containing the connection event information
	 */
	virtual void onConnect(BLEServer* pServer, esp_ble_gatts_cb_param_t *param);

	/**
	 * @brief Handle an existing client disconnection.
	 *
	 * When an existing client disconnects, we are invoked.
	 *
	 * @param [in] pServer A reference to the %BLE server that received the existing client disconnection.
	 */
	virtual void onDisconnect(BLEServer* pServer);
}; // BLEServerCallbacks



#endif /* CONFIG_BT_ENABLED */
#endif /* COMPONENTS_CPP_UTILS_BLESERVER_H_ */
