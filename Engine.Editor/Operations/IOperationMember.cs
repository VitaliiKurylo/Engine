namespace Engine.Editor.Operations
{
    interface IOperationMember
    {
        void OnCommit(object change);
        
        void OnUndo(object change);
        
        void OnRedo(object change);
    }
}
