import React from 'react';
import { useFormikContext } from 'formik';

import AppSlider from '../AppSlider';

function AppFormSlider({ initialValue, minimumValue, maximumValue, name, onValueChange, step, valuePrefix, valueSuffix, width, ...otherProps }) {
    const { setFieldValue, values,  } = useFormikContext();

    return (
        <AppSlider
            minimumValue={minimumValue}
            maximumValue={maximumValue}
            onValueChange={(value) => setFieldValue(name, value)}
            selectedValue={values[name]}
            step={step}
            valuePrefix={valuePrefix}
            valueSuffix={valueSuffix}
            width={width}
            {...otherProps}
        />
    )
}

export default AppFormSlider;