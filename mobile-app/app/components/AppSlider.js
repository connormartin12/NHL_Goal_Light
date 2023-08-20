import React from 'react';
import { StyleSheet, View } from 'react-native';
import Slider from '@react-native-community/slider';

import AppText from './AppText';

function AppSlider({  minimumValue, maximumValue, onValueChange, selectedValue, step, valuePrefix, valueSuffix, width="100%", ...otherProps }) {
    return (
        <View style={styles.container}>
            <AppText style={styles.text}>{valuePrefix} {selectedValue} {valueSuffix}</AppText>
            <Slider
                minimumValue={minimumValue}
                maximumValue={maximumValue}
                onValueChange={onValueChange}
                step={step}
                style={[styles.slider,  { width }]}
                value={selectedValue}
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