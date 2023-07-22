import React from 'react';
import { useFormikContext } from 'formik';

import AppButton from '../AppButton';
import colors from '../../config/colors';

function SubmitButton({ title }) {
    const { handleSubmit } = useFormikContext();

    return (
        <AppButton color={colors.primary} title={title} onPress={handleSubmit}></AppButton>
    );
}

export default SubmitButton;