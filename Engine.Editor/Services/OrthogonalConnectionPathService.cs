using Engine.Editor.Controls;
using Engine.Editor.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;

namespace Engine.Editor.Services
{
    class OrthogonalConnectionPathService : IConnectionPathService
    {
        private const int Margin = 40;
        
        public List<Point> GetConnectionLine(ConnectorInfo source, ConnectorInfo sink, bool showLastLine)
        {
            var linePoints = new List<Point>();

            var rectSource = GetRectWithMargin(source, Margin);
            var rectSink = GetRectWithMargin(sink, Margin);

            var startPoint = GetOffsetPoint(source, rectSource);
            var endPoint = GetOffsetPoint(sink, rectSink);

            linePoints.Add(startPoint);
            var currentPoint = startPoint;

            if (!rectSink.Contains(currentPoint) && !rectSource.Contains(endPoint))
            {
                while (true)
                {
                    #region source node

                    if (IsPointVisible(currentPoint, endPoint, new[] { rectSource, rectSink }))
                    {
                        linePoints.Add(endPoint);
                        currentPoint = endPoint;
                        break;
                    }

                    var neighbour = GetNearestVisibleNeighborSink(currentPoint, endPoint, sink, rectSource, rectSink);
                    if (!double.IsNaN(neighbour.X))
                    {
                        linePoints.Add(neighbour);
                        linePoints.Add(endPoint);
                        currentPoint = endPoint;
                        break;
                    }

                    if (currentPoint == startPoint)
                    {
                        bool flag;
                        var n = GetNearestNeighborSource(source, endPoint, rectSource, rectSink, out flag);
                        linePoints.Add(n);
                        currentPoint = n;

                        if (!IsRectVisible(currentPoint, rectSink, new[] { rectSource }))
                        {
                            Point first, second;
                            GetOppositeCorners(source.Orientation, rectSource, out first, out second);
                            if (flag)
                            {
                                linePoints.Add(first);
                                currentPoint = first;
                            }
                            else
                            {
                                linePoints.Add(second);
                                currentPoint = second;
                            }
                            if (!IsRectVisible(currentPoint, rectSink, new[] { rectSource }))
                            {
                                if (flag)
                                {
                                    linePoints.Add(second);
                                    currentPoint = second;
                                }
                                else
                                {
                                    linePoints.Add(first);
                                    currentPoint = first;
                                }
                            }
                        }
                    }

                    #endregion source node

                    #region sink node

                    else // from here on we jump to the sink node
                    {
                        Point first, second; // neighbour corner
                        Point oppositeFirst, oppositeSecond; // opposite corner
                        GetNeighborCorners(sink.Orientation, rectSink, out oppositeFirst, out oppositeSecond);
                        GetOppositeCorners(sink.Orientation, rectSink, out first, out second);

                        var firstVisible = IsPointVisible(currentPoint, first, new[] { rectSource, rectSink });
                        var secondVisible = IsPointVisible(currentPoint, second, new[] { rectSource, rectSink });

                        if (firstVisible && secondVisible)
                        {
                            if (rectSource.Contains(first))
                            {
                                linePoints.Add(second);
                                if (rectSource.Contains(oppositeSecond))
                                {
                                    linePoints.Add(first);
                                    linePoints.Add(oppositeFirst);
                                }
                                else
                                    linePoints.Add(oppositeSecond);

                                linePoints.Add(endPoint);
                                currentPoint = endPoint;
                                break;
                            }

                            if (rectSource.Contains(second))
                            {
                                linePoints.Add(first);
                                if (rectSource.Contains(oppositeFirst))
                                {
                                    linePoints.Add(second);
                                    linePoints.Add(oppositeSecond);
                                }
                                else
                                    linePoints.Add(oppositeFirst);

                                linePoints.Add(endPoint);
                                currentPoint = endPoint;
                                break;
                            }

                            if ((Distance(first, endPoint) <= Distance(second, endPoint)))
                            {
                                linePoints.Add(first);
                                if (rectSource.Contains(oppositeFirst))
                                {
                                    linePoints.Add(second);
                                    linePoints.Add(oppositeSecond);
                                }
                                else
                                    linePoints.Add(oppositeFirst);
                                linePoints.Add(endPoint);
                                currentPoint = endPoint;
                                break;
                            }
                            linePoints.Add(second);
                            if (rectSource.Contains(oppositeSecond))
                            {
                                linePoints.Add(first);
                                linePoints.Add(oppositeFirst);
                            }
                            else
                                linePoints.Add(oppositeSecond);
                            linePoints.Add(endPoint);
                            currentPoint = endPoint;
                            break;
                        }
                        if (firstVisible)
                        {
                            linePoints.Add(first);
                            if (rectSource.Contains(oppositeFirst))
                            {
                                linePoints.Add(second);
                                linePoints.Add(oppositeSecond);
                            }
                            else
                                linePoints.Add(oppositeFirst);
                            linePoints.Add(endPoint);
                            currentPoint = endPoint;
                            break;
                        }
                        linePoints.Add(second);
                        if (rectSource.Contains(oppositeSecond))
                        {
                            linePoints.Add(first);
                            linePoints.Add(oppositeFirst);
                        }
                        else
                            linePoints.Add(oppositeSecond);
                        linePoints.Add(endPoint);
                        currentPoint = endPoint;
                        break;
                    }

                    #endregion sink node
                }
            }
            else
            {
                linePoints.Add(endPoint);
            }

            linePoints = OptimizeLinePoints(linePoints, new[] { rectSource, rectSink }, source.Orientation,
                sink.Orientation);

            CheckPathEnd(source, sink, showLastLine, linePoints);
            return linePoints;
        }
        
        public List<Point> GetConnectionLine(ConnectorInfo source, Point sinkPoint, ConnectorOrientation preferredOrientation)
        {
            var linePoints = new List<Point>();
            var rectSource = GetRectWithMargin(source, 10);
            var startPoint = GetOffsetPoint(source, rectSource);
            var endPoint = sinkPoint;

            linePoints.Add(startPoint);
            var currentPoint = startPoint;

            if (!rectSource.Contains(endPoint))
            {
                while (true)
                {
                    if (IsPointVisible(currentPoint, endPoint, new[] { rectSource }))
                    {
                        linePoints.Add(endPoint);
                        break;
                    }

                    bool sideFlag;
                    var n = GetNearestNeighborSource(source, endPoint, rectSource, out sideFlag);
                    linePoints.Add(n);
                    currentPoint = n;

                    if (IsPointVisible(currentPoint, endPoint, new[] { rectSource }))
                    {
                        linePoints.Add(endPoint);
                        break;
                    }
                    Point n1, n2;
                    GetOppositeCorners(source.Orientation, rectSource, out n1, out n2);
                    linePoints.Add(sideFlag ? n1 : n2);

                    linePoints.Add(endPoint);
                    break;
                }
            }
            else
            {
                linePoints.Add(endPoint);
            }

            if (preferredOrientation != ConnectorOrientation.None)
                linePoints = OptimizeLinePoints(linePoints, new[] { rectSource }, source.Orientation, preferredOrientation);
            else
                linePoints = OptimizeLinePoints(linePoints, new[] { rectSource }, source.Orientation,
                    GetOpositeOrientation(source.Orientation));

            return linePoints;
        }
        
        private static void CheckPathEnd(ConnectorInfo source, ConnectorInfo sink, bool showLastLine, List<Point> linePoints)
        {
            if (showLastLine)
            {
                var startPoint = new Point(0, 0);
                var endPoint = new Point(0, 0);
                const int marginPath = 10;
                switch (source.Orientation)
                {
                    case ConnectorOrientation.Left:
                        startPoint = new Point(source.Position.X - marginPath, source.Position.Y);
                        break;

                    case ConnectorOrientation.Top:
                        startPoint = new Point(source.Position.X, source.Position.Y - marginPath);
                        break;

                    case ConnectorOrientation.Right:
                        startPoint = new Point(source.Position.X + marginPath, source.Position.Y);
                        break;

                    case ConnectorOrientation.Bottom:
                        startPoint = new Point(source.Position.X, source.Position.Y + marginPath);
                        break;

                    default:
                        break;
                }

                switch (sink.Orientation)
                {
                    case ConnectorOrientation.Left:
                        endPoint = new Point(sink.Position.X - marginPath, sink.Position.Y);
                        break;

                    case ConnectorOrientation.Top:
                        endPoint = new Point(sink.Position.X, sink.Position.Y - marginPath);
                        break;

                    case ConnectorOrientation.Right:
                        endPoint = new Point(sink.Position.X + marginPath, sink.Position.Y);
                        break;

                    case ConnectorOrientation.Bottom:
                        endPoint = new Point(sink.Position.X, sink.Position.Y + marginPath);
                        break;

                    default:
                        break;
                }
                linePoints.Insert(0, startPoint);
                linePoints.Add(endPoint);
            }
            else
            {
                linePoints.Insert(0, source.Position);
                linePoints.Add(sink.Position);
            }
        }
        
        private static double Distance(Point first, Point second) => Point.Subtract(first, second).Length;
        
        private static Point GetNearestNeighborSource(ConnectorInfo source, Point endPoint, Rect rectSource, Rect rectSink, out bool flag)
        {
            Point n1, n2; // neighbors
            GetNeighborCorners(source.Orientation, rectSource, out n1, out n2);

            if (rectSink.Contains(n1))
            {
                flag = false;
                return n2;
            }

            if (rectSink.Contains(n2))
            {
                flag = true;
                return n1;
            }

            if ((Distance(n1, endPoint) <= Distance(n2, endPoint)))
            {
                flag = true;
                return n1;
            }
            flag = false;
            return n2;
        }

        //private static Orientation GetOrientation(ConnectorOrientation sourceOrientation)
        //{
        //    switch (sourceOrientation)
        //    {
        //        case ConnectorOrientation.Left:
        //            return Orientation.Horizontal;
        //        //case ConnectorOrientation.Top:
        //        //    return Orientation.Vertical;
        //        case ConnectorOrientation.Right:
        //            return Orientation.Horizontal;
        //        //case ConnectorOrientation.Bottom:
        //        //    return Orientation.Vertical;
        //        default:
        //            throw new Exception("Unknown ConnectorOrientation");
        //    }
        //}
        
        private static Point GetNearestNeighborSource(ConnectorInfo source, Point endPoint, Rect rectSource, out bool flag)
        {
            Point first, second; // neighbors
            GetNeighborCorners(source.Orientation, rectSource, out first, out second);

            if ((Distance(first, endPoint) <= Distance(second, endPoint)))
            {
                flag = true;
                return first;
            }
            flag = false;
            return second;
        }
        
        private static Point GetNearestVisibleNeighborSink(Point currentPoint, Point endPoint, ConnectorInfo sink, Rect rectSource, Rect rectSink)
        {
            Point s1, s2; // neighbors on sink side
            GetNeighborCorners(sink.Orientation, rectSink, out s1, out s2);

            var flag1 = IsPointVisible(currentPoint, s1, new[] { rectSource, rectSink });
            var flag2 = IsPointVisible(currentPoint, s2, new[] { rectSource, rectSink });

            if (flag1) // s1 visible
            {
                if (flag2) // s1 and s2 visible
                {
                    if (rectSink.Contains(s1))
                        return s2;

                    if (rectSink.Contains(s2))
                        return s1;

                    if ((Distance(s1, endPoint) <= Distance(s2, endPoint)))
                        return s1;
                    return s2;
                }
                return s1;
            }
            if (flag2) // only s2 visible
            {
                return s2;
            }
            return new Point(double.NaN, double.NaN);
        }
        
        private static void GetNeighborCorners(ConnectorOrientation orientation, Rect rect, out Point first, out Point second)
        {
            switch (orientation)
            {
                case ConnectorOrientation.Left:
                    first = rect.TopLeft;
                    second = rect.BottomLeft;
                    break;

                case ConnectorOrientation.Top:
                    first = rect.TopLeft;
                    second = rect.TopRight;
                    break;

                case ConnectorOrientation.Right:
                    first = rect.TopRight;
                    second = rect.BottomRight;
                    break;

                case ConnectorOrientation.Bottom:
                    first = rect.BottomLeft;
                    second = rect.BottomRight;
                    break;

                default:
                    throw new Exception("No neighour corners found!");
            }
        }
        
        private static Point GetOffsetPoint(ConnectorInfo connector, Rect rect)
        {
            var offsetPoint = new Point();

            switch (connector.Orientation)
            {
                case ConnectorOrientation.Left:
                    offsetPoint = new Point(rect.Left, connector.Position.Y);
                    break;

                case ConnectorOrientation.Top:
                    offsetPoint = new Point(connector.Position.X, rect.Top);
                    break;

                case ConnectorOrientation.Right:
                    offsetPoint = new Point(rect.Right, connector.Position.Y);
                    break;

                case ConnectorOrientation.Bottom:
                    offsetPoint = new Point(connector.Position.X, rect.Bottom);
                    break;

                default:
                    break;
            }

            return offsetPoint;
        }
        
        private static ConnectorOrientation GetOpositeOrientation(ConnectorOrientation connectorOrientation)
        {
            switch (connectorOrientation)
            {
                case ConnectorOrientation.Left:
                    return ConnectorOrientation.Right;

                case ConnectorOrientation.Top:
                    return ConnectorOrientation.Bottom;

                case ConnectorOrientation.Right:
                    return ConnectorOrientation.Left;

                case ConnectorOrientation.Bottom:
                    return ConnectorOrientation.Top;

                default:
                    return ConnectorOrientation.Top;
            }
        }
        
        private static void GetOppositeCorners(ConnectorOrientation orientation, Rect rect, out Point first, out Point second)
        {
            switch (orientation)
            {
                case ConnectorOrientation.Left:
                    first = rect.TopRight;
                    second = rect.BottomRight;
                    break;

                case ConnectorOrientation.Top:
                    first = rect.BottomLeft;
                    second = rect.BottomRight;
                    break;

                case ConnectorOrientation.Right:
                    first = rect.TopLeft;
                    second = rect.BottomLeft;
                    break;

                case ConnectorOrientation.Bottom:
                    first = rect.TopLeft;
                    second = rect.TopRight;
                    break;

                default:
                    throw new Exception("No opposite corners found!");
            }
        }
        
        private static ConnectorOrientation GetOrientation(Point first, Point second)
        {
            if (first.X == second.X)
            {
                if (first.Y >= second.Y)
                    return ConnectorOrientation.Top;
                return ConnectorOrientation.Bottom;
            }
            if (first.Y == second.Y)
            {
                if (first.X >= second.X)
                    return ConnectorOrientation.Right;
                return ConnectorOrientation.Left;
            }
            throw new Exception("Failed to retrieve orientation");
        }
        
        private static Rect GetRectWithMargin(ConnectorInfo connectorThumb, double margin)
        {
            var rect = new Rect(connectorThumb.Location, default(Size));

            rect.Inflate(margin, margin);

            return rect;
        }
        
        private static bool IsPointVisible(Point fromPoint, Point targetPoint, Rect[] rectangles) => rectangles.All(rect => !RectangleIntersectsLine(rect, fromPoint, targetPoint));
        
        private static bool IsRectVisible(Point fromPoint, Rect targetRect, Rect[] rectangles)
        {
            if (IsPointVisible(fromPoint, targetRect.TopLeft, rectangles))
                return true;

            if (IsPointVisible(fromPoint, targetRect.TopRight, rectangles))
                return true;

            if (IsPointVisible(fromPoint, targetRect.BottomLeft, rectangles))
                return true;

            if (IsPointVisible(fromPoint, targetRect.BottomRight, rectangles))
                return true;

            return false;
        }
        
        private static List<Point> OptimizeLinePoints(List<Point> linePoints, Rect[] rectangles, ConnectorOrientation sourceOrientation, ConnectorOrientation sinkOrientation)
        {
            var points = new List<Point>();
            var cut = 0;

            for (var i = 0; i < linePoints.Count; i++)
            {
                if (i >= cut)
                {
                    for (var k = linePoints.Count - 1; k > i; k--)
                    {
                        if (IsPointVisible(linePoints[i], linePoints[k], rectangles))
                        {
                            cut = k;
                            break;
                        }
                    }
                    points.Add(linePoints[i]);
                }
            }

            #region Line

            for (var j = 0; j < points.Count - 1; j++)
            {
                if (points[j].X != points[j + 1].X && points[j].Y != points[j + 1].Y)
                {
                    ConnectorOrientation orientationFrom;
                    ConnectorOrientation orientationTo;

                    // orientation from point
                    if (j == 0)
                        orientationFrom = sourceOrientation;
                    else
                        orientationFrom = GetOrientation(points[j], points[j - 1]);

                    // orientation to pint
                    if (j == points.Count - 2)
                        orientationTo = sinkOrientation;
                    else
                        orientationTo = GetOrientation(points[j + 1], points[j + 2]);

                    if ((orientationFrom == ConnectorOrientation.Left || orientationFrom == ConnectorOrientation.Right) &&
                        (orientationTo == ConnectorOrientation.Left || orientationTo == ConnectorOrientation.Right))
                    {
                        var centerX = Math.Min(points[j].X, points[j + 1].X) + Math.Abs(points[j].X - points[j + 1].X) / 2;
                        points.Insert(j + 1, new Point(centerX, points[j].Y));
                        points.Insert(j + 2, new Point(centerX, points[j + 2].Y));
                        if (points.Count - 1 > j + 3)
                            points.RemoveAt(j + 3);
                        return points;
                    }

                    if ((orientationFrom == ConnectorOrientation.Top || orientationFrom == ConnectorOrientation.Bottom) &&
                        (orientationTo == ConnectorOrientation.Top || orientationTo == ConnectorOrientation.Bottom))
                    {
                        var centerY = Math.Min(points[j].Y, points[j + 1].Y) + Math.Abs(points[j].Y - points[j + 1].Y) / 2;
                        points.Insert(j + 1, new Point(points[j].X, centerY));
                        points.Insert(j + 2, new Point(points[j + 2].X, centerY));
                        if (points.Count - 1 > j + 3)
                            points.RemoveAt(j + 3);
                        return points;
                    }

                    if ((orientationFrom == ConnectorOrientation.Left || orientationFrom == ConnectorOrientation.Right) &&
                        (orientationTo == ConnectorOrientation.Top || orientationTo == ConnectorOrientation.Bottom))
                    {
                        points.Insert(j + 1, new Point(points[j + 1].X, points[j].Y));
                        return points;
                    }

                    if ((orientationFrom == ConnectorOrientation.Top || orientationFrom == ConnectorOrientation.Bottom) &&
                        (orientationTo == ConnectorOrientation.Left || orientationTo == ConnectorOrientation.Right))
                    {
                        points.Insert(j + 1, new Point(points[j].X, points[j + 1].Y));
                        return points;
                    }
                }
            }

            #endregion Line

            return points;
        }
        
        private static bool RectangleIntersectsLine(Rect rect, Point startPoint, Point endPoint)
        {
            rect.Inflate(-1, -1);
            return rect.IntersectsWith(new Rect(startPoint, endPoint));
        }
    }
}
