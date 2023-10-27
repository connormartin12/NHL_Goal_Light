import React, { useState } from 'react';
import { Pressable, StyleSheet, View } from 'react-native';

import AppButton from './AppButton';
import { colors } from '../config';
import AppText from './AppText';

const options = ['Off', 'Low', 'High'];

function VolumeSelectionComponent({currentSelection, sendVolumeSetting, width}) {
    const [selectedOption, setSelectedOption] = useState(currentSelection);

    const handlePress = (mode) => {
        console.log(`Audio Setting: ${mode}`);
        if (mode !== "Test")
            setSelectedOption(mode);
        sendVolumeSetting(mode);
    };

    const isSelectionActive = (option) => {
        if (option === selectedOption)
            return true;
        return false;
    };

    return (
        <View style={styles.container}>
            <View style={[styles.optionsContainer, width={width}]}>
                {options.map((option, i) => {
                    return (
                        <Pressable
                            key={i} 
                            onPress={() => handlePress(option)}
                            style={[
                                styles.selection,
                                {
                                    borderColor: isSelectionActive(option) ?
                                    "black" :
                                    colors.inputBackground,
                                },
                            ]}
                        >
                            <AppText style={styles.text}>{option}</AppText>
                        </Pressable>
                    )
                })}
            </View>
            <AppButton 
                onPress={() => handlePress('Test')} title="Test Speaker" width={200} />
        </View>
   );
}

const styles = StyleSheet.create({
    container: {
        alignItems: 'center',
        justifyContent: 'center',
    },
    optionsContainer: {
        flexDirection: 'row',
        justifyContent: 'space-evenly',
        height: 50,
        marginTop: 10,
    },
    selection: {
        alignItems: 'center',
        backgroundColor: colors.inputBackground,
        borderColor: colors.inputBackground,
        borderWidth: 1,
        borderRadius: 8,
        height: '100%',
        justifyContent: 'center',
        width: 80,
    },
    text: {
        color: 'black',
        fontSize: 18,
        textTransform: 'uppercase',
        fontWeight: 'bold',
    },
})

export default VolumeSelectionComponent;