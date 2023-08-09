import React, { useState } from 'react';
import { SafeAreaView, StyleSheet, Text, TouchableOpacity, View } from 'react-native';

import colors from '../config/colors';
import DeviceModal from '../components/DeviceConnectionModal';
import useBLE from '../hooks/useBLE.js';

function DeviceConnectionScreen() {
    const {
        allDevices,
        connectedDevice,
        connectToDevice,
        disconnectFromDevice,
        requestPermissions,
        scanForPeripherals,
    } = useBLE();

    const [isModalVisible, setIsModalVisible] = useState(false);

    const scanForDevices = async () => {
        const isPermissionsEnabled = await requestPermissions();
        if (isPermissionsEnabled) {
            scanForPeripherals();
        }
    };

    const hideModal = () => {
        setIsModalVisible(false);
    };

    const openModal = () => {
        scanForDevices();
        setIsModalVisible(true);
    };

   return (
    <SafeAreaView style={styles.container}>
        <View style={styles.connectionRequest}>
        { connectedDevice ? (
                <Text style={styles.connectionTitleText}>Connected!</Text>
        ) : (
            <Text style={styles.connectionTitleText}>Connect to a Device</Text>)
        }
        
        </View>
            <TouchableOpacity
                onPress={connectedDevice ? disconnectFromDevice : openModal} 
                style={styles.ctaButton} 
            >
            <Text style={styles.ctaButtonText}>
                {connectedDevice ? "Disconnect" : "Connect"}
            </Text>
            </TouchableOpacity>
            <DeviceModal
                closeModal={hideModal}
                visible={isModalVisible}
                connectToPeripheral={connectToDevice}
                devices={allDevices}
            />
    </SafeAreaView>
   );
}

const styles = StyleSheet.create({
    connectionRequest: {
      flex: 1,
      justifyContent: "center",
      alignItems: "center",
    },
    connectionTitleText: {
      fontSize: 30,
      fontWeight: "bold",
      textAlign: "center",
      marginHorizontal: 20,
      color: colors.primary,
    },
    container: {
      flex: 1,
      backgroundColor: colors.inputBackground,
    },
    ctaButton: {
      backgroundColor: colors.primary,
      justifyContent: "center",
      alignItems: "center",
      height: 50,
      marginHorizontal: 20,
      marginBottom: 5,
      borderRadius: 8,
    },
    ctaButtonText: {
      fontSize: 20,
      fontWeight: "bold",
      color: colors.secondary,
    },
  });

export default DeviceConnectionScreen;