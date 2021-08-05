#include "VulkanSwapChain.h"

VulkanSwapChain::VulkanSwapChain(VulkanDevice& r_VulkanDevice, size_t a_FrameAmount)
	:	rm_VulkanDevice(r_VulkanDevice)
{
	SwapChainImages.resize(a_FrameAmount);
	SwapChainImageViews.resize(a_FrameAmount);
	SwapChainFrameBuffers.resize(a_FrameAmount);
}
