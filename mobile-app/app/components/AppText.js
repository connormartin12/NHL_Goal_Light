import React from 'react';
import { Text } from 'react-native';

import { textStyles } from '../config';

function AppText({ children, style, ...otherProps }) {
    return (
        <Text style={[textStyles.default, style]} {...otherProps}>{children}</Text>
    );
}

export default AppText;