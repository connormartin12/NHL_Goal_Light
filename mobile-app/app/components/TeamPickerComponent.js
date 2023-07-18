import React from 'react';
import { StyleSheet, TouchableOpacity } from 'react-native';

import AppText from './AppText';

const TeamPickerComponent = ({name, onPress}) => (
    <TouchableOpacity onPress={onPress} style={styles.listItem}>
        <AppText style={styles.listItemText}>{name}</AppText>
    </TouchableOpacity>
);

const styles = StyleSheet.create({
    listItem: {
        width: '100%',
    },
    listItemText: {
        padding: 10,
    },
})

export default TeamPickerComponent;