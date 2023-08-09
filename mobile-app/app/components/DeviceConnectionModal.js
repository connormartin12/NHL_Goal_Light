import React, { useCallback } from 'react';
import { FlatList, Modal, SafeAreaView, StyleSheet, Text, TouchableOpacity } from 'react-native';

import colors from '../config/colors';

const DeviceModalListItem = (props) => {
    const { item, connectToPeripheral, closeModal } = props;

    const connectAndCloseModal = useCallback(() => {
        connectToPeripheral(item.item);
        closeModal();
    }, [closeModal, connectToPeripheral, item.item]);

    return (
        <TouchableOpacity onPress={connectAndCloseModal} style={modalStyle.ctaButton}>
            <Text style={modalStyle.ctaButtonText}>{item.item.name}</Text>
        </TouchableOpacity>
    );
};

const DeviceModal = (props) => {
    const { devices, visible, connectToPeripheral, closeModal } = props;

    const renderDeviceModalListItem = useCallback((item) => {
        return (
            <DeviceModalListItem 
                item={item} 
                connectToPeripheral={connectToPeripheral} 
                closeModal={closeModal}
            />
        );
    }, [closeModal, connectToPeripheral]
    );

    return (
        <Modal 
            style={modalStyle.modalContainer} 
            animationType="slide" 
            transparent={false} 
            visible={visible}
        >
            <SafeAreaView style={modalStyle.modalTitle}>
                <Text style={modalStyle.modalTitleText}>
                Tap on a device to connect
                </Text>
                <FlatList contentContainerStyle={modalStyle.modalFlatlistContiner} data={devices} renderItem={renderDeviceModalListItem}/>
            </SafeAreaView>
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