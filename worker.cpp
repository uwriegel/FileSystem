#include "worker.h"

void Worker::Execute()
{
	do_work();
};

void Worker::HandleOKCallback()
{
	on_ok(callback);
}
