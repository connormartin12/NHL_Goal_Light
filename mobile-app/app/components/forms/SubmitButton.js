import React from 'react';
import { useFormikContext } from 'formik';

import AppButton from '../AppButton';
import { colors } from '../../config';

function SubmitButton({ title, width, ...otherprops }) {
    const { handleSubmit } = useFormikContext();

    return (
        <AppButton color={colors.primary} title={title} onPress={handleSubmit} width={width} {...otherprops}></AppButton>
    );
}

export default SubmitButton;