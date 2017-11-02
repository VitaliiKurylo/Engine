using System.Collections.Generic;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;
using Engine.Editor.Fluent;
using System.Linq;
using Engine.Editor.Framework;
using Engine.Editor.Models;

namespace Engine.Editor.Controls
{
    class SchemeCanvas : AbstractCanvas
    {
        private List<Connector> _connectorsHit;
        private ConnectorModel _partialConnector;
        private Connector _sourceConnector;
        
        public SchemeCanvas()
        {
            _connectorsHit = new List<Connector>();
        }
        
        public Connector SourceConnector
        {
            get { return _sourceConnector; }
            set
            {
                _sourceConnector.Not().Null(() =>
                {
                    _sourceConnector = value;
                    _connectorsHit.Add(_sourceConnector);
                    var fullyCreatedConnectorModel = _sourceConnector.DataContext as FullyCreatedConnectorModel;
                    var rectangleBounds = _sourceConnector.TransformToVisual(this).TransformBounds(new Rect(_sourceConnector.RenderSize));
                    var point = new Point(rectangleBounds.Left + (rectangleBounds.Width / 2d), rectangleBounds.Bottom + (rectangleBounds.Height / 2d));
                    _partialConnector = new ConnectorModel(fullyCreatedConnectorModel, new PartiallyCreatedConnectorModel(point));

                    fullyCreatedConnectorModel.Target.Parent.AddElement.Execute(_partialConnector);
                });
            }
        }
        
        protected override void HitTesting(Point hitPoint)
        {
            var @object = InputHitTest(hitPoint) as DependencyObject;
            while (ReferenceEquals(@object, null) == false && @object.GetType().Equals(GetType()) == false)
            {
                if (@object is Connector)
                {
                    if (_connectorsHit.Contains((Connector)@object) == false)
                        _connectorsHit.Add((Connector)@object);
                }
                @object = VisualTreeHelper.GetParent(@object);
            }
        }
        
        protected override void OnMouseDown(MouseButtonEventArgs eventArguments)
        {
            base.OnMouseDown(eventArguments);

            if (eventArguments.LeftButton == MouseButtonState.Pressed && eventArguments.Source == this)
            {
                var projectDocument = (IProjectDocument)DataContext;

                if (Keyboard.Modifiers.HasFlag(ModifierKeys.Control) == false)
                    projectDocument.ClearSelection.Execute(null);

                eventArguments.Handled = true;
            }
        }
        
        protected override void OnMouseMove(MouseEventArgs eventArguments)
        {
            base.OnMouseMove(eventArguments);

            SourceConnector.Not().Null(() =>
            {
                if (eventArguments.LeftButton == MouseButtonState.Pressed)
                {
                    var currentPoint = eventArguments.GetPosition(this);
                    _partialConnector.SinkConnectorModel = new PartiallyCreatedConnectorModel(currentPoint);
                    HitTesting(currentPoint);
                }
            });

            eventArguments.Handled = true;
        }
        
        protected override void OnMouseUp(MouseButtonEventArgs eventArguments)
        {
            base.OnMouseUp(eventArguments);

            _sourceConnector.Not().Null(() =>
            {
                var fullyCreatedConnectorModel = _sourceConnector.DataContext as FullyCreatedConnectorModel;
                var partiallyCreatedConnector = fullyCreatedConnectorModel.Target.Parent.Elements.OfType<ConnectorModel>().LastOrDefault((element) => element.IsFullConnection == false);
                fullyCreatedConnectorModel.Target.Parent.RemoveElement.Execute(partiallyCreatedConnector);

                if (_connectorsHit.Count() == 2)
                {
                    var sinkDataItem = _connectorsHit.Last().DataContext as FullyCreatedConnectorModel;
                    
                    sinkDataItem.Target.Parent.AddElement.Execute(new ConnectorModel(fullyCreatedConnectorModel, sinkDataItem));
                }
            });

            _connectorsHit.Clear();
            _sourceConnector = null;
        }
    }
}
