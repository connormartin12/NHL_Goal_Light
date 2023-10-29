import React from 'react';
import { Modal, StyleSheet, View } from 'react-native';
import LottieView from 'lottie-react-native';

import AppText from './AppText';

function SpeakerIndicator({visible = false, closeSpeakerIndicator}) {
    return (
        <Modal 
            animationType="fade"
            style={styles.container}
            transparent={true}
            visible={visible}
        >
            <View style={styles.modalContainer}>
                <AppText style={styles.text}>Playing Audio</AppText>
                <LottieView 
                    autoPlay={true}
                    loop={false}
                    onAnimationFinish={closeSpeakerIndicator}
                    source={require('../assets/animations/loading-bar.json')}
                    style={{
                    height: 18,
                }}
                />
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

export default SpeakerIndicator;