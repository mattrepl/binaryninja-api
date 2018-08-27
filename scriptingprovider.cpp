#include "binaryninjaapi.h"

using namespace BinaryNinja;
using namespace std;


ScriptingOutputListener::ScriptingOutputListener()
{
	m_callbacks.context = this;
	m_callbacks.output = OutputCallback;
	m_callbacks.error = ErrorCallback;
	m_callbacks.inputReadyStateChanged = InputReadyStateChangedCallback;
}


void ScriptingOutputListener::OutputCallback(void* ctxt, const char* text)
{
	ScriptingOutputListener* listener = (ScriptingOutputListener*)ctxt;
	listener->NotifyOutput(text);
}


void ScriptingOutputListener::ErrorCallback(void* ctxt, const char* text)
{
	ScriptingOutputListener* listener = (ScriptingOutputListener*)ctxt;
	listener->NotifyError(text);
}


void ScriptingOutputListener::InputReadyStateChangedCallback(void* ctxt, BNScriptingProviderInputReadyState state)
{
	ScriptingOutputListener* listener = (ScriptingOutputListener*)ctxt;
	listener->NotifyInputReadyStateChanged(state);
}


void ScriptingOutputListener::NotifyOutput(const string&)
{
}


void ScriptingOutputListener::NotifyError(const string&)
{
}


void ScriptingOutputListener::NotifyInputReadyStateChanged(BNScriptingProviderInputReadyState)
{
}


ScriptingInstance::ScriptingInstance(ScriptingProvider* provider)
{
	BNScriptingInstanceCallbacks cb;
	cb.context = this;
	cb.destroyInstance = DestroyInstanceCallback;
	cb.executeScriptInput = ExecuteScriptInputCallback;
	cb.setCurrentBinaryView = SetCurrentBinaryViewCallback;
	cb.setCurrentFunction = SetCurrentFunctionCallback;
	cb.setCurrentBasicBlock = SetCurrentBasicBlockCallback;
	cb.setCurrentAddress = SetCurrentAddressCallback;
	cb.setCurrentSelection = SetCurrentSelectionCallback;
	AddRefForRegistration();
	m_object = BNInitScriptingInstance(provider->GetObject(), &cb);
}


ScriptingInstance::ScriptingInstance(BNScriptingInstance* instance)
{
	m_object = instance;
}


ScriptingInstance::~ScriptingInstance()
{
	BNFreeScriptingInstance(m_object);
}


void ScriptingInstance::DestroyInstanceCallback(void* ctxt)
{
	ScriptingInstance* instance = (ScriptingInstance*)ctxt;
	instance->DestroyInstance();
}


BNScriptingProviderExecuteResult ScriptingInstance::ExecuteScriptInputCallback(void* ctxt, const char* input)
{
	ScriptingInstance* instance = (ScriptingInstance*)ctxt;
	return instance->ExecuteScriptInput(input);
}


void ScriptingInstance::SetCurrentBinaryViewCallback(void* ctxt, BNBinaryView* view)
{
	ScriptingInstance* instance = (ScriptingInstance*)ctxt;
	instance->SetCurrentBinaryView(view ? new BinaryView(BNNewViewReference(view)) : nullptr);
}


void ScriptingInstance::SetCurrentFunctionCallback(void* ctxt, BNFunction* func)
{
	ScriptingInstance* instance = (ScriptingInstance*)ctxt;
	instance->SetCurrentFunction(func ? new Function(BNNewFunctionReference(func)) : nullptr);
}


void ScriptingInstance::SetCurrentBasicBlockCallback(void* ctxt, BNBasicBlock* block)
{
	ScriptingInstance* instance = (ScriptingInstance*)ctxt;
	instance->SetCurrentBasicBlock(block ? new BasicBlock(BNNewBasicBlockReference(block)) : nullptr);
}


void ScriptingInstance::SetCurrentAddressCallback(void* ctxt, uint64_t addr)
{
	ScriptingInstance* instance = (ScriptingInstance*)ctxt;
	instance->SetCurrentAddress(addr);
}


void ScriptingInstance::SetCurrentSelectionCallback(void* ctxt, uint64_t begin, uint64_t end)
{
	ScriptingInstance* instance = (ScriptingInstance*)ctxt;
	instance->SetCurrentSelection(begin, end);
}


void ScriptingInstance::DestroyInstance()
{
}


void ScriptingInstance::SetCurrentBinaryView(BinaryView*)
{
}


void ScriptingInstance::SetCurrentFunction(Function*)
{
}


void ScriptingInstance::SetCurrentBasicBlock(BasicBlock*)
{
}


void ScriptingInstance::SetCurrentAddress(uint64_t)
{
}


void ScriptingInstance::SetCurrentSelection(uint64_t, uint64_t)
{
}


void ScriptingInstance::Output(const string& text)
{
	BNNotifyOutputForScriptingInstance(m_object, text.c_str());
}


void ScriptingInstance::Error(const string& text)
{
	BNNotifyErrorForScriptingInstance(m_object, text.c_str());
}


void ScriptingInstance::InputReadyStateChanged(BNScriptingProviderInputReadyState state)
{
	BNNotifyInputReadyStateForScriptingInstance(m_object, state);
}


BNScriptingProviderInputReadyState ScriptingInstance::GetInputReadyState()
{
	return BNGetScriptingInstanceInputReadyState(m_object);
}


void ScriptingInstance::RegisterOutputListener(ScriptingOutputListener* listener)
{
	BNRegisterScriptingInstanceOutputListener(m_object, &listener->GetCallbacks());
}


void ScriptingInstance::UnregisterOutputListener(ScriptingOutputListener* listener)
{
	BNUnregisterScriptingInstanceOutputListener(m_object, &listener->GetCallbacks());
}


CoreScriptingInstance::CoreScriptingInstance(BNScriptingInstance* instance): ScriptingInstance(instance)
{
}


BNScriptingProviderExecuteResult CoreScriptingInstance::ExecuteScriptInput(const string& input)
{
	return BNExecuteScriptInput(m_object, input.c_str());
}


void CoreScriptingInstance::SetCurrentBinaryView(BinaryView* view)
{
	BNSetScriptingInstanceCurrentBinaryView(m_object, view ? view->GetObject() : nullptr);
}


void CoreScriptingInstance::SetCurrentFunction(Function* func)
{
	BNSetScriptingInstanceCurrentFunction(m_object, func ? func->GetObject() : nullptr);
}


void CoreScriptingInstance::SetCurrentBasicBlock(BasicBlock* block)
{
	BNSetScriptingInstanceCurrentBasicBlock(m_object, block ? block->GetObject() : nullptr);
}


void CoreScriptingInstance::SetCurrentAddress(uint64_t addr)
{
	BNSetScriptingInstanceCurrentAddress(m_object, addr);
}


void CoreScriptingInstance::SetCurrentSelection(uint64_t begin, uint64_t end)
{
	BNSetScriptingInstanceCurrentSelection(m_object, begin, end);
}


ScriptingProvider::ScriptingProvider(const string& name): m_nameForRegister(name)
{
}


ScriptingProvider::ScriptingProvider(BNScriptingProvider* provider)
{
	m_object = provider;
}


BNScriptingInstance* ScriptingProvider::CreateInstanceCallback(void* ctxt)
{
	ScriptingProvider* provider = (ScriptingProvider*)ctxt;
	Ref<ScriptingInstance> instance = provider->CreateNewInstance();
	return instance ? BNNewScriptingInstanceReference(instance->GetObject()) : nullptr;
}


string ScriptingProvider::GetName()
{
	char* providerNameRaw = BNGetScriptingProviderName(m_object);
	string providerName(providerNameRaw);
	BNFreeString(providerNameRaw);
	return providerName;
}


vector<Ref<ScriptingProvider>> ScriptingProvider::GetList()
{
	size_t count;
	BNScriptingProvider** list = BNGetScriptingProviderList(&count);
	vector<Ref<ScriptingProvider>> result;
	for (size_t i = 0; i < count; i++)
		result.push_back(new CoreScriptingProvider(list[i]));
	BNFreeScriptingProviderList(list);
	return result;
}


Ref<ScriptingProvider> ScriptingProvider::GetByName(const string& name)
{
	BNScriptingProvider* result = BNGetScriptingProviderByName(name.c_str());
	if (!result)
		return nullptr;
	return new CoreScriptingProvider(result);
}


void ScriptingProvider::Register(ScriptingProvider* provider)
{
	BNScriptingProviderCallbacks cb;
	cb.context = provider;
	cb.createInstance = CreateInstanceCallback;
	provider->m_object = BNRegisterScriptingProvider(provider->m_nameForRegister.c_str(), &cb);
}


CoreScriptingProvider::CoreScriptingProvider(BNScriptingProvider* provider): ScriptingProvider(provider)
{
}


Ref<ScriptingInstance> CoreScriptingProvider::CreateNewInstance()
{
	BNScriptingInstance* result = BNCreateScriptingProviderInstance(m_object);
	if (!result)
		return nullptr;
	return new CoreScriptingInstance(result);
}
