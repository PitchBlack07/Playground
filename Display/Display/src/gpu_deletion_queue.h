#pragma once
#ifndef GPU_DELETION_QUEUE_H_INCUDED
#define GPU_DELETION_QUEUE_H_INCUDED

#include <d3d12.h>

struct gpu_deletion_queue
{
	UINT64 frameId;
};

//gpu_deletion_queue* gpu_deletion_queue_create(UINT64 frameId_);

#endif
