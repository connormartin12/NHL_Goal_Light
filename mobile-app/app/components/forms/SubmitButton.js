import React from 'react';
import { useFormikContext } from 'formik';

import AppButton from '../AppButton';
import { colors } from '../../config';

function SubmitButton({ title, ...otherprops }) {
    const { handleSubmit } = useFormikContext();

    return (
        <AppButton color={colors.primary} title={title} onPress={handleSubmit} {...otherprops}></AppButton>
    );
}

export default SubmitButton;