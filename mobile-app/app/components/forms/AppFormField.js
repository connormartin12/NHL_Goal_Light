import React from 'react';
import { useFormikContext } from 'formik';

import AppTextInput from '../AppTextInput';
import ErrorMessage from './ErrorMessage';

function AppFormField({ marginTop, name, onPress, passwordField, width, ...otherProps }) {
    const { setFieldTouched, setFieldValue, errors, touched, values} = useFormikContext();

    return (
        <>
            <AppTextInput
                marginTop={marginTop}
                onBlur={() => setFieldTouched(name)}
                onChangeText={text => setFieldValue(name, text)}
                onPress={onPress}
                passwordField={passwordField}
                value={values[name]}
                width={width}
                {...otherProps}
            />
            <ErrorMessage error={errors[name]} visible={touched[name]} />
        </>
    );
}

export default AppFormField;