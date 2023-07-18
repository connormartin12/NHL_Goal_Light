import React from 'react';
import { useFormikContext } from 'formik';

import AppSlider from '../AppSlider';

function AppFormSlider({ initialValue, minimumValue, maximumValue, name, step, valuePrefix, valueSuffix, width, ...otherProps }) {
    const { setFieldValue } = useFormikContext();

    return (
        <AppSlider
            initialValue={initialValue}
            minimumValue={minimumValue}
            maximumValue={maximumValue}
            onSlidingComplete={(value) => setFieldValue(name, value)}
            step={step}
            valuePrefix={valuePrefix}
            valueSuffix={valueSuffix}
            width={width}
            {...otherProps}
        />
    )
}

export default AppFormSlider;