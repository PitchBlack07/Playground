
#include <libvk.h>
#include <assert.h>

VkResult libvk_get_supported_extensions(const char* layer_, VkExtensionProperties** propertiesOut_, uint32_t* countOut_)
{
    if (propertiesOut_ == NULL || countOut_ == NULL) {
        return VK_INCOMPLETE;
    }

    uint32_t count = 0;
    vkEnumerateInstanceExtensionProperties(layer_, &count, NULL);
    if (count) {
        VkExtensionProperties* const properties = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * count);
        if (properties) {
            VkResult res = vkEnumerateInstanceExtensionProperties(layer_, &count, properties);
            if (res == VK_SUCCESS) {
                *countOut_      = count;
                *propertiesOut_ = properties;
            }
            else {
                free(properties);
                *propertiesOut_ = NULL;
                *countOut_      = 0;
            }
            return res;
        }
    }
    return VK_SUCCESS;
}

VkBool32 libvk_is_extension_supported(const char* ext_, const VkExtensionProperties* extensions_, uint32_t count_)
{
    if (ext_ == NULL) {
        return VK_FALSE;
    }

    for (uint32_t i = 0; i < count_; ++i) {
        if (strcmp(ext_, extensions_[i].extensionName) == 0) {
            return VK_TRUE;
        }
    }
    return VK_FALSE;
}

VkResult libvk_choose_supported_extensions(const char* layer_, const char** extensionsInOut_, uint32_t* countIntOut_)
{
    uint32_t supportedCount = 0;
    uint32_t checkCount     = *countIntOut_;

    VkExtensionProperties* extensions = NULL;

    VkResult res = libvk_get_supported_extensions(layer_, &extensions, &supportedCount);
    if (res != VK_SUCCESS) {
        return res;
    }

    uint32_t i, ridx = 0, widx = 0;

    for (i = 0; i < checkCount; ++i) {
        if (libvk_is_extension_supported(extensionsInOut_[ridx], extensions, supportedCount)) {
            extensionsInOut_[widx++] = extensionsInOut_[ridx++];
        }
        else {
            extensionsInOut_[ridx++] = NULL;
        }
    }
    *countIntOut_ = widx;

    free(extensions);

    return VK_SUCCESS;
}

VkResult libvk_get_supported_layers(VkLayerProperties** propertiesOut_, uint32_t* countOut_)
{
    if (propertiesOut_ == NULL || countOut_ == NULL) {
        return VK_INCOMPLETE;
    }

    uint32_t count = 0;
    vkEnumerateInstanceLayerProperties(&count, NULL);
    if (count) {
        VkLayerProperties* const properties = (VkLayerProperties*)malloc(sizeof(VkLayerProperties) * count);
        if (properties) {
            VkResult res = vkEnumerateInstanceLayerProperties(&count, properties);
            if (res == VK_SUCCESS) {
                *countOut_      = count;
                *propertiesOut_ = properties;
            }
            else {
                free(properties);
                *propertiesOut_ = NULL;
                *countOut_      = 0;
            }
            return res;
        }
    }
    return VK_SUCCESS;
}

VkBool32 libvk_is_layer_supported(const char* layer_, const VkLayerProperties* layers_, uint32_t count_)
{
    if (layer_ == NULL) {
        return VK_FALSE;
    }

    for (uint32_t i = 0; i < count_; ++i) {
        if (strcmp(layer_, layers_[i].layerName) == 0) {
            return VK_TRUE;
        }
    }
    return VK_FALSE;
}

VkResult libvk_choose_supported_layers(const char** layersInOut_, uint32_t* countInOut_)
{
    uint32_t supportedCount = 0;
    uint32_t checkCount     = *countInOut_;

    VkLayerProperties* layers = NULL;

    VkResult res = libvk_get_supported_layers(&layers, &supportedCount);
    if (res != VK_SUCCESS) {
        return res;
    }

    uint32_t i, ridx = 0, widx = 0;

    for (i = 0; i < checkCount; ++i) {
        if (libvk_is_layer_supported(layersInOut_[ridx], layers, supportedCount)) {
            layersInOut_[widx++] = layersInOut_[ridx++];
        }
        else {
            layersInOut_[ridx++] = NULL;
        }
    }
    *countInOut_ = widx;

    free(layers);

    return VK_SUCCESS;
}

VkResult libvk_choose_supported_extensions2(const char* layer_, uint32_t extensionCount_, const char* const* extensions_, uint32_t* bufferCountInOut_, const char** extensionsOut_)
{
    uint32_t i, j, k;
    uint32_t count   = 0;
    VkResult success = VK_SUCCESS;

    VkExtensionProperties  buffer[16] = { 0 };
    VkExtensionProperties* p          = NULL;

    success = vkEnumerateInstanceExtensionProperties(layer_, &count, NULL);
    if (success != VK_SUCCESS)
        goto failed;

    p = libvk_alloc_conditional(sizeof(VkExtensionProperties) * count, buffer, sizeof(buffer));
    if (p == NULL) {
        success = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto failed;
    }

    success = vkEnumerateInstanceExtensionProperties(layer_, &count, p);
    if (success != VK_SUCCESS)
        goto failed;

    k = 0;
    for (i = 0; i < extensionCount_; ++i) {
        for (j = 0; j < count; ++j) {
            if (strcmp(extensions_[i], p[j].extensionName) == 0) {
                if (k >= *bufferCountInOut_) {
                    success = VK_INCOMPLETE;
                    goto failed;
                }

                extensionsOut_[k++] = extensions_[i];
                break;
            }
        }
    }

	*bufferCountInOut_ = k;

    assert(success == VK_SUCCESS);

failed:
    libvk_free_conditional(p, buffer);
    return success;
}

libvk_application_t* libvk_create_application(const char* name_, uint32_t layerCount_, const char* const* layers_, uint32_t extensionCount_, const char* const* extensions_)
{
    VkApplicationInfo    ai;
    VkInstanceCreateInfo ci;

    libvk_application_t* app                 = NULL;
    char*                extbuffer[32]       = { 0 };
    const char**         supportedExtensions = (const char**)libvk_alloc_conditional(sizeof(const char*) * extensionCount_, extbuffer, sizeof(extbuffer));
    uint32_t             supportedExtCount   = extensionCount_;

    if (supportedExtensions == NULL && extensionCount_ != 0) {
        goto failed;
    }

    libvk_choose_supported_extensions2(NULL, extensionCount_, extensions_, &supportedExtCount, supportedExtensions);

    app = (libvk_application_t*)malloc(sizeof(libvk_application_t));
    if (!app) {
        goto failed;
    }

    memset(app, 0, sizeof(*app));

    ai.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    ai.pNext              = NULL;
    ai.apiVersion         = VK_MAKE_VERSION(1, 0, 0);
    ai.applicationVersion = 1;
    ai.engineVersion      = 1;
    ai.pApplicationName   = name_;
    ai.pEngineName        = name_;

    ci.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    ci.pNext                   = NULL;
    ci.flags                   = 0;
    ci.pApplicationInfo        = &ai;
    ci.enabledLayerCount       = layerCount_;
    ci.ppEnabledLayerNames     = layers_;
    ci.enabledExtensionCount   = extensionCount_;
    ci.ppEnabledExtensionNames = extensions_;

    if (vkCreateInstance(&ci, NULL, &app->vkInstance) != VK_SUCCESS) {
        free(app);
        app = NULL;
    }

failed:
    libvk_free_conditional((void*)supportedExtensions, extbuffer);
    return app;
}

void livk_destroy_application(libvk_application_t* app_)
{
    app_;
}