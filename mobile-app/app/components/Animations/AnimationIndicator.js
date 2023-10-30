import React from 'react';
import { Modal, StyleSheet, View } from 'react-native';
import SpeakerAnimation from './SpeakerAnimation';
import ConnectingIndicator from './ConnectingIndicator';

function AnimationIndicator({Animation=ConnectingIndicator, closeAnimation, visible }) {
    return (
        <Modal 
            animationType="fade"
            style={styles.container}
            transparent={true}
            visible={visible}
        >
            <View style={styles.modalContainer}>
                <Animation closeIndicator={closeAnimation} visible={visible} />
            </View>
        </Modal>
    );
}

const styles=StyleSheet.create({
    container: {
        flex: 1,
    }, 
    modalContainer: {
        alignItems: 'center',
        backgroundColor: 'rgba(211, 211, 211, 0.9)',
        flex: 1,
        gap: 20,
        justifyContent: 'center',
    },
    text: {
        fontSize: 24,
        fontWeight: 'bold',
    }
});

export default AnimationIndicator;