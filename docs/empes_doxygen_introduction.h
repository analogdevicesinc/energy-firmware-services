/******************************************************************************
 Copyright (c) 2023 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @mainpage Energy Firmware Services Documentation
 *
 * @section intro Introduction
 * Energy Firmware Services is a suite of hardware-agnostic software modules designed for energy firmware development.
 * These modules utilize dependency injection APIs to interface with hardware-dependent functionality.
 * The services follow the ADI Energy Firmware Architecture, where each service interacts with hardware peripherals
 * through APIs provided by the application. The service API typically defines an `ADI_XX_CONFIG` structure and
 * corresponding `ADI_XX_YY_FUNC` function pointers for peripheral access.
 *
 * \image html "service_architecture.png"
 *
 * As illustrated above, the application is responsible for initializing and configuring the required peripherals.
 * Callback functions should be invoked from the appropriate ISRs.
 *
 * @section init Service Initialization and Configuration
 * The generic API sequence for using a service is as follows:
 *   - @b adi_service_Create: Creates an instance of the service.
 *   - @b adi_service_Init: Initializes and configures the service.
 *   - @b adi_service_SetConfig: Reconfigures the service if needed.
 *
 * To configure the service, fill the @b ADI_SERVICE_CONFIG structure and pass it to the initialization API.
 *   - @b ADI_XX_CONFIG.pfXxYy: Pointer to the function that provides access to the peripheral.
 *
 * Example initialization and configuration sequence:
 * @code
 * #include <adi_service.h>
 *
 * adi_service_Create();
 * // Fill the configuration structure before initialization
 * adi_service_Init();
 * // Call appropriate APIs as required by the application.
 * @endcode
 *
 * @section docs Documentation
 * @subsection api_doc API Documentation
 * The Energy Firmware Service documentation is available in HTML format, generated from Doxygen comments in the codebase.
 *
 * @section support Technical or Customer Support
 * For support, contact Analog Devices, Inc.:
 *   - Email: energy.support@analog.com
 *
 * @section copyright Copyright
 * Copyright (c) 2023 - 2025  Analog Devices Inc.
 */

/*
** EOF
*/

/*@}*/
