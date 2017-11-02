using System;

namespace Engine.Editor.Framework
{
    interface IGroupable
    {
        Guid Id { get; }
        
        Guid ParentId { get; set; }
        
        bool IsGrouped { get; set; }
    }
}
