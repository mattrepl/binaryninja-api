#pragma once

#include <QtWidgets/QWidget>
#include <QtCore/QMetaType>
#include <QtWidgets/QMainWindow>
#include <QtGui/QWheelEvent>
#include "binaryninjaapi.h"
#include "action.h"
#include "preview.h"

#define PREVIEW_HOVER_TIME 500

typedef bool (*UIPluginInitFunction)(void);
typedef void (*UIPluginDependencyFunction)(void);

class ViewFrame;
class UIActionHandler;
class FileContext;

class BINARYNINJAUIAPI UIContextNotification
{
public:
	virtual void OnContextOpen(UIContext* context) { (void)context; }
	virtual void OnContextClose(UIContext* context) { (void)context; }

	virtual bool OnBeforeOpenDatabase(UIContext* context, FileMetadataRef metadata) { (void)context; (void)metadata; return true; }
	virtual bool OnAfterOpenDatabase(UIContext* context, FileMetadataRef metadata, BinaryViewRef data) { (void)context; (void)metadata; (void)data; return true; }
	virtual bool OnBeforeOpenFile(UIContext* context, FileContext* file) { (void)context; (void)file; return true; }
	virtual void OnAfterOpenFile(UIContext* context, FileContext* file, ViewFrame* frame) { (void)context; (void)file; (void)frame; }
	virtual bool OnBeforeSaveFile(UIContext* context, FileContext* file, ViewFrame* frame) { (void)context; (void)file; (void)frame; return true; }
	virtual void OnAfterSaveFile(UIContext* context, FileContext* file, ViewFrame* frame) { (void)context; (void)file; (void)frame; }
	virtual bool OnBeforeCloseFile(UIContext* context, FileContext* file, ViewFrame* frame) { (void)context; (void)file; (void)frame; return true; }
	virtual void OnAfterCloseFile(UIContext* context, FileContext* file, ViewFrame* frame) { (void)context; (void)file; (void)frame; }

	virtual void OnViewChange(UIContext* context, ViewFrame* frame, const QString& type) { (void)context; (void)frame; (void)type; }
};

class BINARYNINJAUIAPI UIContextHandler
{
public:
	virtual ~UIContextHandler();
	virtual void updateStatus(bool updateInfo) = 0;
	virtual void notifyThemeChanged() = 0;
	virtual void registerFileOpenMode(const QString& buttonName, const QString& description, const QString& action);
};

class BINARYNINJAUIAPI UIContext
{
	static UIContextHandler* m_handler;
	static std::set<UIContext*> m_contexts;
	UIActionHandler m_globalActions;
	static std::vector<UIContextNotification*> m_notifications;

	static QPointer<PreviewWidget> m_currentPreview;

protected:
	void setupUIContext(QWidget* obj);

public:
	UIContext();
	virtual ~UIContext();

	virtual QMainWindow* mainWindow() = 0;
	virtual void viewChanged(ViewFrame* frame, const QString& type);
	virtual bool navigateForBinaryView(BinaryViewRef view, uint64_t addr);
	virtual void createTabForWidget(const QString& name, QWidget* widget) = 0;
	virtual bool openFilename(const QString& path, bool openOptions = false);
	virtual ViewFrame* openFileContext(FileContext* file, const QString& forcedView = "", bool addTab = true);

	UIActionHandler* globalActions() { return &m_globalActions; }
	virtual UIActionHandler* contentActionHandler() = 0;

	static void registerNotification(UIContextNotification* notification);
	static void unregisterNotification(UIContextNotification* notification);

	void NotifyOnContextOpen();
	void NotifyOnContextClose();

	bool NotifyOnBeforeOpenDatabase(FileMetadataRef metadata);
	bool NotifyOnAfterOpenDatabase(FileMetadataRef metadata, BinaryViewRef data);
	bool NotifyOnBeforeOpenFile(FileContext* file);
	void NotifyOnAfterOpenFile(FileContext* file, ViewFrame* frame);
	bool NotifyOnBeforeSaveFile(FileContext* file, ViewFrame* frame);
	void NotifyOnAfterSaveFile(FileContext* file, ViewFrame* frame);
	bool NotifyOnBeforeCloseFile(FileContext* file, ViewFrame* frame);
	void NotifyOnAfterCloseFile(FileContext* file, ViewFrame* frame);

	void NotifyOnViewChange(ViewFrame* frame, const QString& type);

	static void setHandler(UIContextHandler* handler);

	static QSize getScaledWindowSize(int x, int y);
	static void updateStatus(bool updateInfo = true);
	static void notifyThemeChanged();
	static void showPreview(QWidget* parent, PreviewWidget* preview, QPoint localPos);
	static void closePreview();
	static bool sendPreviewWheelEvent(QWheelEvent* event);
	static void closeOtherActiveModalWidget(QWidget* current);
	static void registerFileOpenMode(const QString& buttonName, const QString& description, const QString& action);

	static UIContext* contextForWidget(QWidget* widget);
	static UIContext* activeContext();
	static std::set<UIContext*> allContexts();
};

Q_DECLARE_METATYPE(UIContext*)

void BINARYNINJAUIAPI InitUIViews();
void BINARYNINJAUIAPI InitUIActions();

void BINARYNINJAUIAPI InitUIPlugins();
void BINARYNINJAUIAPI SetCurrentUIPluginLoadOrder(BNPluginLoadOrder order);
void BINARYNINJAUIAPI AddRequiredUIPluginDependency(const std::string& name);
void BINARYNINJAUIAPI AddOptionalUIPluginDependency(const std::string& name);
