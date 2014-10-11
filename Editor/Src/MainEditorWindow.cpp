

#include "maineditorwindow.h"
#include "DebugLogger.h"
#include "HierarchyModel.h"
#include "GameObject.h"
#include "ui_maineditorwindow.h"
#include "Rendering\RenderManager.h"
#include "Scene\Scene.h"
#include "Widgets\SceneViewWidget.h"
#include "Widgets\TransformWidget.h"

#include <QtWidgets>

using namespace EditorCommands;

MainEditorWindow::MainEditorWindow(QWidget *parent)
: m_ui(new Ui::MainEditorWindow)
{
    // Window setup
    m_ui->setupUi(this);
    setWindowTitle(tr("Dogwood Editor!"));

    // Tree view setup
    m_view = m_ui->treeView;

    // Scene view widget setup
    m_sceneViewWidget = new SceneViewWidget(this);
    m_sceneViewWidget->setFixedSize(640, 480);
    m_ui->verticalLayout->addWidget(m_sceneViewWidget);

    // Component widgets
    m_transformWidget = new TransformWidget(this, this);
    m_ui->componentLayout->addWidget(m_transformWidget);
    m_transformWidget->hide();

    // Debug logging
    DebugLogger::Singleton().SetTextEditTarget(m_ui->textEdit_DebugOutput);

    // File menu
    connect(m_ui->actionNew_Scene, SIGNAL(triggered()), this, SLOT(NewScene()));
    connect(m_ui->actionOpen_Scene, SIGNAL(triggered()), this, SLOT(OpenScene()));
    connect(m_ui->actionSave_Scene, SIGNAL(triggered()), this, SLOT(SaveScene()));
    connect(m_ui->actionSave_Scene_As, SIGNAL(triggered()), this, SLOT(SaveSceneAs()));

    // Edit menu
    connect(m_ui->actionUndo,               SIGNAL(triggered()), this, SLOT(Undo()));
    connect(m_ui->actionRedo,               SIGNAL(triggered()), this, SLOT(Redo()));

    // Game Object menu
    connect(m_ui->actionCreate_Game_Object, SIGNAL(triggered()), this, SLOT(CreateGameObject()));
    connect(m_ui->actionDelete_Game_Object, SIGNAL(triggered()), this, SLOT(DeleteGameObject()));
    connect(m_ui->actionCopy_Game_Object,   SIGNAL(triggered()), this, SLOT(CopyGameObject()));
    connect(m_ui->actionCut_Game_Object,    SIGNAL(triggered()), this, SLOT(CutGameObject()));
    connect(m_ui->actionPaste_Game_Object,  SIGNAL(triggered()), this, SLOT(PasteGameObject()));

    // Debug menu
    connect(m_ui->actionOpen_Test_Scene, SIGNAL(triggered()), this, SLOT(OpenTestScene()));
}

MainEditorWindow::~MainEditorWindow()
{
    delete m_ui;
    delete m_sceneViewWidget;
}

void MainEditorWindow::Paint()
{
    m_sceneViewWidget->update();
}

void MainEditorWindow::SetHierarchyModel(HierarchyModel* model)
{
    m_model = model;
    m_view->setModel(m_model);
    DebugLogger::Singleton().Log("Hierarchy model set!");
    connect(m_view->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(OnSelectionChanged(const QItemSelection &, const QItemSelection &)));
    m_view->expandAll();
}

// TODO: Qt textboxes seem to capture ctrl + z and do their own undo. stop that!
// TODO: just selecting a game object counts as a bunch of update commands. fix that!
void MainEditorWindow::Undo()
{
    // TODO disable menu option if undo is not currently available
    bool success = CommandManager::Singleton().Undo();

    if (success)
    {
        DebugLogger::Singleton().Log("Undo");
    }
    else
    {
        DebugLogger::Singleton().Log("Can't undo. Stack is empty.");
    }
}

void MainEditorWindow::Redo()
{
    // TODO disable menu option if redo is not currently available
    bool success = CommandManager::Singleton().Redo();

    if (success)
    {
        DebugLogger::Singleton().Log("Redo");
    }
    else
    {
        DebugLogger::Singleton().Log("Can't redo. Stack is empty.");
    }
}

void MainEditorWindow::NewScene()
{
    // TODO implement me
    DebugLogger::Singleton().Log("New scene: not implemented yet");
}

void MainEditorWindow::OpenScene()
{
    DebugLogger::Singleton().Log("Open scene");

    // Open file dialog
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Scene"), "", tr("Scene Files (*.xml)"));

    // TODO unload previous scene
    m_scene = new Scene();
    if (m_scene->LoadScene(fileName.toStdString()))
    {
        HierarchyModel* model = new HierarchyModel(m_scene->GetRootObject());
        SetHierarchyModel(model);
        RenderManager::Singleton().SetRootObject(m_scene->GetRootObject());
    }
    else
    {
        DebugLogger::Singleton().Log("Error loading scene");
        m_scene = NULL;
    }
}

void MainEditorWindow::OpenTestScene()
{
    // TODO unload previous scene
    m_scene = new Scene();
    if (m_scene->LoadScene("..\\Game\\Assets\\Scenes\\Scene0.xml"))
    {
        HierarchyModel* model = new HierarchyModel(m_scene->GetRootObject());
        SetHierarchyModel(model);
        RenderManager::Singleton().SetRootObject(m_scene->GetRootObject());
    }
}

void MainEditorWindow::SaveScene()
{
    DebugLogger::Singleton().Log("Save scene");

    if (m_scene != NULL)
    {
        m_scene->SaveScene();
    }
}

void MainEditorWindow::SaveSceneAs()
{
    DebugLogger::Singleton().Log("Save scene as...");

    if (m_scene != NULL)
    {
        // Open file dialog
        QString fileName = QFileDialog::getSaveFileName(this, "Save Scene As", "", "*.xml");
        m_scene->SaveScene(fileName.toStdString());
    }
    else
    {
        DebugLogger::Singleton().Log("No scene to save.");
    }
}

void MainEditorWindow::CreateGameObject()
{
    DebugLogger::Singleton().Log("Creating game object");

    QModelIndex index = m_view->selectionModel()->currentIndex();
    CreateGameObjectCommand* command = new CreateGameObjectCommand(m_model, m_view, index);
    CommandManager::Singleton().ExecuteCommand(command);
}

void MainEditorWindow::DeleteGameObject()
{
    DebugLogger::Singleton().Log("Deleting game object");

    QModelIndex index = m_view->selectionModel()->currentIndex();
    DeleteGameObjectCommand* command = new DeleteGameObjectCommand(m_model, m_view, index);
    CommandManager::Singleton().ExecuteCommand(command);
}

void MainEditorWindow::CopyGameObject()
{
    QModelIndex index = m_view->selectionModel()->currentIndex();

    m_copiedGameObject = index.isValid() ? m_model->getItem(index) : NULL;

    if (m_copiedGameObject != NULL)
    {
        DebugLogger::Singleton().Log("Copied game object");
    }
    else
    {
        DebugLogger::Singleton().Log("Tried to copy game object but selection was invalid.");
    }
}

void MainEditorWindow::CutGameObject()
{
    QModelIndex index = m_view->selectionModel()->currentIndex();

    m_copiedGameObject = index.isValid() ? m_model->getItem(index) : NULL;

    if (m_copiedGameObject != NULL)
    {
        DebugLogger::Singleton().Log("Cutting game object");

        DeleteGameObjectCommand* command = new DeleteGameObjectCommand(m_model, m_view, index);
        CommandManager::Singleton().ExecuteCommand(command);
    }
    else
    {
        DebugLogger::Singleton().Log("Tried to cut object but selection was invalid.");
    }
}

void MainEditorWindow::PasteGameObject()
{
    if (m_copiedGameObject != NULL)
    {
        DebugLogger::Singleton().Log("Pasting game object");

        QModelIndex index = m_view->selectionModel()->currentIndex();
        PasteGameObjectCommand* command = new PasteGameObjectCommand(m_model, m_view, index, m_copiedGameObject);
        CommandManager::Singleton().ExecuteCommand(command);
    }
    else
    {
        DebugLogger::Singleton().Log("Tried to paste but there was no copied object.");
    }
}

// TODO: discard trivial updates that don't actually modify values
void MainEditorWindow::UpdateGameObjectTransform(Vector3 vector, VectorType type)
{
    DebugLogger::Singleton().Log("Updating gameobject transform");

    QModelIndex index = m_view->selectionModel()->currentIndex();
    ModifyTransformCommand* command = new ModifyTransformCommand(m_model, index, vector, type);
    CommandManager::Singleton().ExecuteCommand(command);
}

void MainEditorWindow::OnSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
    // TODO enforce selection of only one object at a time?

    // Get the selected game object
    const QModelIndex index = m_view->selectionModel()->currentIndex();
    GameObject* go = m_model->getItem(index);

    if (go != NULL)
    {
        // Show the components for the selected game object
        m_transformWidget->SetGameObject(go);
        m_transformWidget->show();
    }
    else
    {
        // Nothing is selected, so hide widgets
        m_transformWidget->SetGameObject(NULL);
        m_transformWidget->hide();
    }
}

void MainEditorWindow::resizeEvent(QResizeEvent* resizeEvent)
{
    QSize newSize = resizeEvent->size();
    int width = newSize.width();
    int height = newSize.height();

    DebugLogger::Singleton().Log("Window resize!");

    //TODO: scale/reposition widgets
}