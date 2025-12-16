#include "DeviceListView.h"

DeviceListView::DeviceListView()
    : environment(nullptr), rows(0), cols(0)
{

}

DeviceListView::DeviceListView(
    MeshtasticEnvironment *env
)
    : environment(env), rows(0), cols(0)
{
}

DeviceListView::~DeviceListView()
{
}
