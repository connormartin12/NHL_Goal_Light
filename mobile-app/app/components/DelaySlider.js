import React, { useState } from 'react';
import { StyleSheet, View } from 'react-native';
import Slider from '@react-native-community/slider';

import AppText from './AppText';

function DelaySlider() {
    const [delay, setDelay] = useState(30);

    return (
        <View style={styles.container}>
            <AppText style={styles.text}>Goal Light Delay: {delay} Seconds</AppText>
            <Slider
                minimumValue={0}
                maximumValue={60}
                onValueChange={(value) => setDelay(value)}
                step={1}
                style={styles.slider}
                value={delay}
            />
        </View>    
    );
}

const styles = StyleSheet.create({
    container: {
        alignItems: 'center',
        marginTop: 15,
        width: '100%',
    },
    slider: {
        height: 40,
        marginTop: 10,
        width: '100%',
    },
    text: {
        fontWeight: 500,
    }
})

export default DelaySlider;