import React from 'react';
import { Modal, StyleSheet, View } from 'react-native';
import ConnectingIndicator from './ConnectingIndicator';

import { colors } from '../../config';

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
        backgroundColor: colors.animationBackground,
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