#ifndef MMR_COMMON_POLICY_CONTAINER_H
#define MMR_COMMON_POLICY_CONTAINER_H

#include <common/include/Common_def.h>
/*
	策略容器定义
*/
BEGINE_NAMESPACE(mmrComm)

template <typename...TPolicies>
struct PolicyContainer;

template <typename T>
constexpr bool IsPolicyContainer = false;

template <typename...T>
constexpr bool IsPolicyContainer<PolicyContainer<T...>> = true;

template <typename TLayerName, typename...TPolicies>
struct SubPolicyContainer;

template <typename T>
constexpr bool IsSubPolicyContainer = false;

template <typename TLayer, typename...T>
constexpr bool IsSubPolicyContainer<SubPolicyContainer<TLayer, T...>> = true;

END_NAMESPACE(mmrComm)

#endif
