import React, { useState } from 'react';
import { StyleSheet, View } from 'react-native';
import Slider from '@react-native-community/slider';

import AppText from './AppText';

function AppSlider({ onSlidingComplete, initialValue, minimumValue, maximumValue, step, valuePrefix, valueSuffix, width="100%", ...otherProps }) {
    const [sliderValue, setSliderValue] = useState(initialValue);

    return (
        <View style={styles.container}>
            <AppText style={styles.text}>{valuePrefix} {sliderValue} {valueSuffix}</AppText>
            <Slider
                minimumValue={minimumValue}
                maximumValue={maximumValue}
                onValueChange={(value) => setSliderValue(value)}
                onSlidingComplete={onSlidingComplete}
                step={step}
                style={[styles.slider,  { width }]}
                value={sliderValue}
                {...otherProps}
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
    },
    text: {
        fontWeight: 500,
    }
})

export default AppSlider;