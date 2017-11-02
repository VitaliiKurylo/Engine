using Engine.Editor.Extensions;
using System.Windows.Controls;
using System.Windows.Input;
using Engine.Editor.Fluent;

namespace Engine.Editor.Controls
{
    enum ConnectorOrientation : byte
    {
        None = 0,
        Left,
        Right,
        Top,
        Bottom
    }

    class Connector : Control
    {
        public ConnectorOrientation Orientation { get; set; }
        
        protected override void OnMouseLeftButtonDown(MouseButtonEventArgs eventArguments)
        {
            base.OnMouseLeftButtonDown(eventArguments);
            this.GetParent<SchemeCanvas>().Not().Null((canvas) => canvas.SourceConnector = this);
        }
    }
}
