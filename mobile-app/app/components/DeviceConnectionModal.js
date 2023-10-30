import React, { useCallback } from 'react';
import { Button, FlatList, Modal, StyleSheet, Text, TouchableOpacity } from 'react-native';

import colors from '../config/colors';
import Screen from './Screen';

const DeviceModalListItem = (props) => {
    const { closeModal, connectToPeripheral, item, showConnectingIndicator } = props;

    const connectAndCloseModal = useCallback(() => {
        showConnectingIndicator();
        connectToPeripheral(item.item);
        closeModal();
    }, [closeModal, connectToPeripheral, item.item, showConnectingIndicator]);

    return (
        <TouchableOpacity 
            onPress={() => {
            connectAndCloseModal();
            }} 
            style={modalStyle.ctaButton}>
            <Text style={modalStyle.ctaButtonText}>{item.item.name}</Text>
        </TouchableOpacity>
    );
};

const DeviceModal = (props) => {
    const { closeModal, connectToPeripheral, devices, showConnectingIndicator, visible } = props;

    const renderDeviceModalListItem = useCallback((item) => {
        return (
            <DeviceModalListItem 
                closeModal={closeModal}
                connectToPeripheral={connectToPeripheral} 
                item={item} 
                showConnectingIndicator={showConnectingIndicator}
            />
        );
    }, [closeModal, connectToPeripheral]
    );

    return (
        <Modal 
            animationType="slide" 
            style={modalStyle.modalContainer} 
            transparent={false} 
            visible={visible}
        >
            <Screen style={modalStyle.modalTitle}>
                <Button title="Close" onPress={closeModal} />
                <Text style={modalStyle.modalTitleText}>
                    Tap on a device to connect
                </Text>
                <FlatList 
                    contentContainerStyle={modalStyle.modalFlatlistContiner}
                    data={devices}
                    renderItem={renderDeviceModalListItem}
                />
            </Screen>
        </Modal>
    );
};

const modalStyle = StyleSheet.create({
    modalContainer: {
        flex: 1,
        backgroundColor: colors.inputBackground,
    },
    modalFlatlistContiner: {
        flex: 1,
        justifyContent: "center",
    },
    modalCellOutline: {
        alignItems: "center",
        marginHorizontal: 20,
        paddingVertical: 15,
        borderRadius: 8,
    },
    modalTitle: {
        flex: 1,
        backgroundColor: colors.inputBackground,
    },
    modalTitleText: {
        marginTop: 40,
        fontSize: 30,
        fontWeight: "bold",
        marginHorizontal: 20,
        textAlign: "center",
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
        fontSize: 18,
        fontWeight: "bold",
        color: colors.secondary,
    },
});

export default DeviceModal;