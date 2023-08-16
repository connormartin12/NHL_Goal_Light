import React from 'react';
import { Formik } from 'formik';

function AppForm({ enableReinitialize, initialValues, onSubmit, children, validationSchema }) {
    return (
        <Formik
            enableReinitialize={enableReinitialize}
            initialValues={initialValues}
            onSubmit={onSubmit}
            validationSchema={validationSchema}
        >
            { () => <>{children}</>}
        </Formik>
    )
}

export default AppForm;