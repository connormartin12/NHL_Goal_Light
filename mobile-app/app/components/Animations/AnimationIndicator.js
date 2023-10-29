import React from 'react';
import { Modal, StyleSheet, View } from 'react-native';
import LottieView from 'lottie-react-native';
import SpeakerAnimation from './SpeakerAnimation';

function AnimationIndicator({closeAnimation, visible }) {
    return (
        <Modal 
            animationType="fade"
            style={styles.container}
            transparent={true}
            visible={visible}
        >
            <View style={styles.modalContainer}>
                <SpeakerAnimation closeSpeakerIndicator={closeAnimation} visible={visible} />
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