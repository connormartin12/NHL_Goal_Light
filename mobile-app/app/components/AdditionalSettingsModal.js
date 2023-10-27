import React, { useCallback } from 'react';
import { Alert, Button, Modal, StyleSheet } from 'react-native';
import { useFormikContext } from 'formik';

import AppButton from './AppButton';
import colors from '../config/colors';
import Screen from './Screen';
import VolumeSelectionComponent from './VolumeSelectionComponent';
import AppText from './AppText';

const maxScreenWidth = 300;

function AdditionalSettingsModal(props) {
    const { closeModal, disconnectFromPeripheral, espVolume, resetPeripheral, sendVolumeSetting, visible } = props;
    const { resetForm } = useFormikContext();

    const resetAndCloseModal = useCallback(() => {
        Alert.alert('Reset Goal Light', 'Are you sure you want to reset your goal light?', [
            {
                text: 'Reset Device',
                onPress: () => {
                    resetPeripheral();
                    disconnectFromPeripheral();
                    closeModal();
                    resetForm();
                }
            },
            {
                text: 'Cancel',
            }
        ]);
    }, [closeModal, disconnectFromPeripheral, resetForm, resetPeripheral]);

    return (
        <Modal
            animationType="slide"
            style={styles.container}
            transparent={false}
            visible={visible}
        >
            <Screen style={styles.modalContainer}>
                <Button title="Close" onPress={closeModal} />
                <AppText style={styles.sectionHeader}>Audio Settings</AppText>
                <VolumeSelectionComponent 
                    currentSelection={espVolume}
                    sendVolumeSetting={sendVolumeSetting}
                    width={maxScreenWidth}/>
                <AppButton 
                    color="red"
                    onPress={resetAndCloseModal}
                    title="Reset Device"
                    width={maxScreenWidth} 
                />
            </Screen>
        </Modal>
    );
}

const styles = StyleSheet.create({
    container: {
        flex: 1,
        backgroundColor: colors.inputBackground,
    },
    sectionHeader: {
        fontSize: 24,
        fontWeight: '700',
        alignSelf: 'flex-start',
    },
    modalContainer: {
        alignItems: 'center',
        flex: 1,
    },
})

export default AdditionalSettingsModal;