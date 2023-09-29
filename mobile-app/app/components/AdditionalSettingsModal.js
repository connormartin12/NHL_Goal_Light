import React from 'react';
import { Button, Modal, StyleSheet, View } from 'react-native';

import AppButton from './AppButton';
import colors from '../config/colors';
import Screen from './Screen';

function AdditionalSettingsModal(props) {
    const { closeModal, visible } = props;

    const reset = () => {

    };

    return (
        <Modal
            animationType="slide"
            style={styles.container}
            transparent={false}
            visible={visible}
        >
            <Screen style={styles.modalContainer}>
                <Button title="Close" onPress={closeModal} />
                <AppButton 
                    color="red"
                    onPress={reset}
                    title="Reset Device"
                    width={300} />
            </Screen>
        </Modal>
    );
}

const styles = StyleSheet.create({
    container: {
        flex: 1,
        backgroundColor: colors.inputBackground,
    },
    modalContainer: {
        alignItems: 'center',
        flex: 1,
    },
})

export default AdditionalSettingsModal;