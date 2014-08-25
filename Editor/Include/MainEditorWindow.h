#pragma once

#include <qabstractitemmodel.h>
#include <QItemSelection>
#include <QMainWindow>
#include <string>

#include "CommandManager.h"
#include "EditorCommands.h"
#include "Math\Algebra.h"

class GLWidget;
class GameObject;
class HierarchyModel;
class QTreeView;
class TransformWidget;

using std::string;

namespace Ui
{
    class MainEditorWindow;
}

class MainEditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainEditorWindow(QWidget *parent = 0);
    ~MainEditorWindow();

    void Paint();

    void SetHierarchyModel(HierarchyModel* model);
    void DebugLog(string text);

    void UpdateGameObjectTransform(Vector3 vector, VectorType type);

private:
    Ui::MainEditorWindow*   m_ui;
    GLWidget*               m_glWidget;
    TransformWidget*        m_transformWidget;

    QTreeView*              m_view;
    HierarchyModel*         m_model;

    CommandManager          m_commandManager;

private slots:
    void CreateGameObject();
    void DeleteGameObject();
    void Undo();
    void Redo();
    void OnSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
};