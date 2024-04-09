Option Explicit On

Imports System.IO

Imports System.Drawing
Imports System.Drawing.Drawing2D


Public Class Form1

    Public Const HDR_FLX_SIZE = 128
    Public Const HDR_SHP_SIZE = 6

    ' HDR FLX file Info
    Public Structure s_HDR_FLX
        Dim hdr_1A_82() As Byte     ' 82 Bytes
        Dim hdr_zero_2() As Byte    '  2 Bytes
        Dim hdr_nclss As UInt16     '  2 Bytes
        Dim hdr_v As UInt32         '  4 Bytes
        Dim hdr_fs As UInt32        '  4 Bytes
        Dim hdr_crc As UInt32       '  4 Bytes
        Dim hdr_zero_28() As Byte   ' 28 Bytes
    End Structure

    ' DATA FLX SHAPE Info
    Public Structure s_FLX_Table_Shapes
        Dim Offset As UInt32
        Dim Size As UInt32
        Dim Relative_FrameNumber As UInt16
        Dim Absolute_FrameNumber As UInt16
    End Structure

    ' SHAPE HEADER Info
    Public Structure s_HDR_SHP
        Dim MaxWidth As UInt16               ' Max Width of each FRAME
        Dim MaxHeight As UInt16              ' Max Height of each FRAME
        Dim NumberofFrames As UInt16         ' Easy, total number of FRAMES for a given SHAPE
        Dim a_TBL_HDR_SHP() As s_TBL_HDR_SHP  ' TABLE OF FRAMES inside SHP to the FRAME
    End Structure

    ' Each TABLE OF FRAMES part of the SHAPE HDR
    Public Structure s_TBL_HDR_SHP
        Dim frame_realOffset As UInt16 ' Real readed Offset.
        Dim frame_NumSubSHP As Byte
        Dim frame_MultiSHP As Byte
        Dim frame_Size As UInt16
        Dim frame_Offset As UInt32 ' Calculated Offset for own purposes (can get 32bytes addresses)
    End Structure

    'Individual FRAME Information
    Public Structure s_FRAME
        Dim Type As UInt16
        Dim FrameNumber As UInt16
        Dim Unknown As UInt32
        Dim Compression As UInt16
        Dim frm_Width As UInt16      ' XLength
        Dim frm_Height As UInt16     ' YLength (fH)
        Dim x_Offset As UInt16          ' Top Y height position of Frame (from Image Top, -0, y_Top-       : like (0, 0))
        Dim y_Offset As UInt16       ' Bottom Y height position of Frame (from Image Top, -0, y_Bottom- : like (0, 8))
        Dim table_Lines() As UInt16  ' (fH) * 2
        Dim DATAIMAGE() As Byte     ' Unknown size right now.
    End Structure

    Public HDR_FLX As s_HDR_FLX
    Public FLX_Table_Shapes() As s_FLX_Table_Shapes
    Public HDR_Shapes As s_HDR_SHP
    Public HDR_Frame() As s_FRAME
    Public num_Shapes As Short, num_Shapes_Abs As Short
    Public num_Frames As Short
    Public Shared s_RGBPAL(255) As Color
    Public PALLoaded As Boolean

    ' Other Vars.
    Public Path As String
    Private Filename As String
    Private FileLoaded As Boolean
    Private FileLoadedU8FONTS As Boolean
    Private FileLoadedU8GUMPS As Boolean
    Private Exporting_Frame As Boolean
    Private Shape_Loaded As Boolean
    Public Frame_Imported As Boolean
    Public ShapeType As Integer
    Public ShapeSize As Long
    'Public ZOOM As Integer
    Dim x_loc_ori As Long, y_loc_ori As Long

    Public Function GetRealShape(ByRef Value As Long) As Integer
        Dim pos As Integer
        Dim Found As Boolean

        pos = 0

        If Not Shape_Loaded Then
            While Not Found
                If FLX_Table_Shapes(pos).Absolute_FrameNumber = Value Then
                    Found = True
                End If
                pos = pos + 1
            End While

            If pos > 0 Then pos = pos - 1
        End If

        Return pos
    End Function

    Public Sub ResizeImage(ByVal pbFrame As PictureBox, ByVal image As Image, ByVal BMPSize As Size)
        Dim newWidth As Integer
        Dim newHeight As Integer

        If Not Exporting_Frame Then
            If cbAZ.Checked Then
                ' Let's calculate the max zoom. 'Max zoom is 10.
                Dim max_zoom As Integer
                Dim Found As Boolean
                max_zoom = 0 : Found = False

                While max_zoom < 11 And Not Found
                    If (BMPSize.Width * max_zoom) > 640 Or (BMPSize.Height * max_zoom) > 480 Then
                        Found = True
                        Exit While
                    End If
                    max_zoom = max_zoom + 1
                End While

                tbZoom.Value = max_zoom - 1
            End If


            newWidth = BMPSize.Width * tbZoom.Value
            newHeight = BMPSize.Height * tbZoom.Value
        Else
            newWidth = BMPSize.Width
            newHeight = BMPSize.Height
        End If

        Dim newImage As Image = New Bitmap(newWidth, newHeight)

        Using graphicsHandle As Graphics = Graphics.FromImage(newImage)
            graphicsHandle.SmoothingMode = SmoothingMode.None
            graphicsHandle.InterpolationMode = InterpolationMode.NearestNeighbor
            graphicsHandle.PixelOffsetMode = Drawing2D.PixelOffsetMode.HighQuality
            graphicsHandle.DrawImage(image, 0, 0, newWidth, newHeight)
        End Using

        pbFrame.Width = newWidth
        pbFrame.Height = newHeight

        If Not Exporting_Frame Then
            pbFrameBorder.Width = newWidth + 4
            pbFrameBorder.Height = newHeight + 4

            ' Calculate Location
            Dim x_loc_new As Long, y_loc_new As Long
            x_loc_new = x_loc_ori + (320 - (newWidth / 2))
            y_loc_new = y_loc_ori + (240 - (newHeight / 2))

            ' Put PictureBox at Location
            pbFrame.Location = New Point(x_loc_new, y_loc_new)
            pbFrameBorder.Location = New Point(x_loc_new - 2, y_loc_new - 2)
        End If

        ' Put Bitmap
        pbFrame.Image = newImage.Clone

        newImage.Dispose()

    End Sub

    Public Sub BMPDrawLine(ByVal BMP As Bitmap, ByRef x0 As Long, ByRef y0 As Long, ByRef x1 As Long, ByRef y1 As Long, ByRef cColor As Color)
        Dim y_aux As Long

        y_aux = y0
        While x0 <= x1
            While y0 <= y1
                BMP.SetPixel(x0, y0, cColor)
                y0 = y0 + 1
            End While
            x0 = x0 + 1
            y0 = y_aux
        End While
    End Sub

    Public Sub DrawFrame(ByVal pbFrame As PictureBox, ByVal sel_Frame As Long)
        Dim l_frm_x As Long, l_frm_y As Long, l_offset As Long
        Dim x_pos As Long, y_pos As Long, y_len As Long, data_len As Long
        Dim i As Long
        Dim cColor As Color
        Dim BMPSize As Size

        Try
            ' Var image size for multiple purposes
            BMPSize.Width = HDR_Frame(sel_Frame).frm_Width
            BMPSize.Height = HDR_Frame(sel_Frame).frm_Height

            ' Graphical Objects
            Dim sBrush As SolidBrush = New SolidBrush(Color.Black)
            Dim P As Pen = New Pen(sBrush)

            Dim BMP As Bitmap = New Bitmap(BMPSize.Width, BMPSize.Height)

            'G.SmoothingMode = Drawing2D.SmoothingMode.None
            Dim G As Graphics = Graphics.FromImage(BMP)
            G.SmoothingMode = SmoothingMode.None
            G.InterpolationMode = InterpolationMode.NearestNeighbor
            G.PixelOffsetMode = Drawing2D.PixelOffsetMode.HighQuality
            'G.InterpolationMode = Drawing2D.InterpolationMode.HighQualityBicubic

            ' Clear and Prepare PictureBoxes.
            pbFrame.Image = Nothing

            ' Prepare Background with transparent Color.
            ' Let's draw transparent pixels (index 255 of U8PAL.PAL).
            If cbTransparency.Checked And Not Exporting_Frame Then
                sBrush.Color = Color.Transparent
            Else
                sBrush.Color = s_RGBPAL(255)
            End If

            G.FillRectangle(sBrush, 0, 0, BMPSize.Width, BMPSize.Height)

            ' Let's draw the FRAME
            ' Initial preparations
            y_len = HDR_Frame(sel_Frame).frm_Height
            l_frm_x = 0 'HDR_Frame(sel_Frame).x_Offset
            l_frm_y = 0 'HDR_Frame(sel_Frame).y_Offset
            x_pos = HDR_Frame(sel_Frame).frm_Width
            y_pos = -1

            ' Formula to draw Frame.
            Do
                Do Until x_pos < HDR_Frame(sel_Frame).frm_Width
                    y_pos = y_pos + 1

                    If y_pos = HDR_Frame(sel_Frame).frm_Height Then Exit Do

                    l_offset = HDR_Frame(sel_Frame).table_Lines(y_pos) - ((HDR_Frame(sel_Frame).frm_Height - y_pos) * 2)
                    x_pos = HDR_Frame(sel_Frame).DATAIMAGE(l_offset)
                    l_offset = l_offset + 1
                Loop

                If y_pos = y_len Then Exit Do

                data_len = HDR_Frame(sel_Frame).DATAIMAGE(l_offset)
                l_offset = l_offset + 1

                If HDR_Frame(sel_Frame).Compression = 1 Then
                    If (data_len And 1) = 1 Then
                        data_len = data_len \ 2

                        cColor = s_RGBPAL(HDR_Frame(sel_Frame).DATAIMAGE(l_offset))
                        l_offset = l_offset + 1
                        BMPDrawLine(BMP, x_pos + l_frm_x, y_pos + l_frm_y, x_pos + data_len - 1 + l_frm_x, y_pos + l_frm_y, cColor)
                    Else
                        data_len = data_len \ 2

                        For i = x_pos To x_pos + data_len - 1
                            cColor = s_RGBPAL(HDR_Frame(sel_Frame).DATAIMAGE(l_offset))
                            l_offset = l_offset + 1
                            BMP.SetPixel(i + l_frm_x, y_pos + l_frm_y, cColor)
                        Next i
                    End If
                Else
                    For i = x_pos To x_pos + data_len - 1
                        cColor = s_RGBPAL(HDR_Frame(sel_Frame).DATAIMAGE(l_offset))
                        l_offset = l_offset + 1
                        BMP.SetPixel(i + l_frm_x, y_pos + l_frm_y, cColor)
                    Next i
                End If

                x_pos = x_pos + data_len
                If x_pos < HDR_Frame(sel_Frame).frm_Width Then
                    x_pos = x_pos + HDR_Frame(sel_Frame).DATAIMAGE(l_offset)
                    l_offset = l_offset + 1
                End If
            Loop

            ' Ok, Supossedly, image created. Let's put it in PictureBox
            ' resizing it.
            ResizeImage(pbFrame, BMP.Clone, BMPSize)

            ' Free everything
            P.Dispose()
            sBrush.Dispose()
            BMP.Dispose()
        Catch
            MsgBox("Failed on Drawing Frame " + sel_Frame.ToString + ".")
        End Try
    End Sub

    Public Sub ReadUniqueFrame(ByVal sel_Frame As Long, ByVal sel_Shape As Long, ByVal fileStream As FileStream, ByVal readFLX As BinaryReader)
        Dim size_DATAIMAGE As Long

        Try
            ' If Size of Frame is 0 (or less), the image has probably no Data Frame
            If HDR_Shapes.a_TBL_HDR_SHP(sel_Frame).frame_Size < 1 Then Return

            ' Ok, we have the Frame.
            ' Let's read FRAME HDR.
            With HDR_Frame(sel_Frame)
                ' Get Type of Frame (Only for Shapes.flx?) - 2Bytes
                .Type = readFLX.ReadUInt16

                ' Get Frame Number (Only for Shapes.flx?) - 2Bytes
                .FrameNumber = readFLX.ReadUInt16

                ' Get Unkown Data - 4Bytes
                .Unknown = readFLX.ReadUInt32

                ' Get Compression Type - 2Bytes
                .Compression = readFLX.ReadUInt16

                ' Get Width of Frame (x) - 2Bytes
                .frm_Width = readFLX.ReadUInt16

                ' Get Height of Frame (y) - 2Bytes
                .frm_Height = readFLX.ReadUInt16

                ' Get y_Top (This would be Top Y point of image (Top Left 0,YTop point)) - 2Bytes
                .x_Offset = readFLX.ReadUInt16

                ' Get y_Bottom (This would be Bottom Y poing of image (Bottom 0,YBottom point)) - 2Byte
                .y_Offset = readFLX.ReadUInt16
            End With

            ' ReDim Table of Lines
            ReDim HDR_Frame(sel_Frame).table_Lines(HDR_Frame(sel_Frame).frm_Height - 1)

            ' Let's get each Line of the Shape and put it in the Table of Lines.
            For i = 0 To HDR_Frame(sel_Frame).frm_Height - 1
                HDR_Frame(sel_Frame).table_Lines(i) = readFLX.ReadUInt16
            Next

            ' Let's ReDim it first to the size needed
            ' If Frame is compressed, we need to get different size of Bytes for DATAIMAGE
            If FileLoadedU8FONTS Then
                ' Reading from U8FONTS.FLX
                size_DATAIMAGE = HDR_Shapes.a_TBL_HDR_SHP(sel_Frame).frame_Size - ((HDR_Frame(sel_Frame).frm_Height * 2) + 18)
            Else
                If FileLoadedU8GUMPS Then
                    ' Reading from U8GUMPS.FLX
                    size_DATAIMAGE = HDR_Shapes.a_TBL_HDR_SHP(sel_Frame).frame_Size - (HDR_Frame(sel_Frame).frm_Height * 2) - 10
                Else
                    ' Reading from U8SHAPES.FLX supposedly
                    size_DATAIMAGE = HDR_Shapes.a_TBL_HDR_SHP(sel_Frame).frame_Size
                End If
            End If

            ReDim HDR_Frame(sel_Frame).DATAIMAGE(size_DATAIMAGE)

            ' We read DATAIMAGE
            HDR_Frame(sel_Frame).DATAIMAGE = readFLX.ReadBytes(size_DATAIMAGE)

            ' Done reading FRAME!
        Catch
            MsgBox("Failed to Read Frame " + sel_Frame.ToString + "from FLX file.")
        End Try
    End Sub

    Public Sub ReadShape(ByVal sel_Shape As Long)
        Dim i As Long
        Dim shape_offset As UInt32

        ' Working with file
        Dim fileStream As New FileStream(Filename, FileMode.Open)
        Dim readFLX As BinaryReader = New BinaryReader(fileStream)

        Try
            ' Go to the Offset of the Frame
            ' Maybe we are reading the shape and not the .FLX file.
            If Shape_Loaded Then
                shape_offset = 0
            Else
                shape_offset = FLX_Table_Shapes(sel_Shape).Offset
            End If

            If (shape_offset > 0 Or Shape_Loaded) Then
                fileStream.Seek(shape_offset, SeekOrigin.Begin)

                ' Ok, We need to read HDR Shape Information
                With HDR_Shapes
                    .MaxWidth = readFLX.ReadUInt16
                    .MaxHeight = readFLX.ReadUInt16
                    .NumberofFrames = readFLX.ReadUInt16
                End With

                ' Assign number of Frames to var (the one in FLX, although maybe they are used not all of them).
                ' In Spanish .FLXs maybe the 1 frame shapes has numberofshapes = 0. Let's modify this by code.
                If HDR_Shapes.NumberofFrames = 0 Then
                    num_Frames = 1
                Else
                    num_Frames = HDR_Shapes.NumberofFrames
                End If

                ' Redim Number of frames array.
                ReDim HDR_Shapes.a_TBL_HDR_SHP(num_Frames - 1)

                ' We need to read the Table of Frames (Struct with Offset, Compress, Size)
                ' We are now finished reading Header of the Shape we have selected.
                ' Now we should update Frame ListBox.
                For i = 0 To (num_Frames - 1)
                    ' Read Data Frame values for Shape.
                    With HDR_Shapes.a_TBL_HDR_SHP(i)
                        .frame_realOffset = readFLX.ReadUInt16
                        .frame_NumSubSHP = readFLX.ReadByte
                        .frame_MultiSHP = readFLX.ReadByte
                        .frame_Size = readFLX.ReadUInt16
                        .frame_Offset = .frame_realOffset + (&HFFFF * .frame_NumSubSHP) + .frame_NumSubSHP
                        'End If
                    End With
                Next

                ' Ok, now we need to load in memory each Frame, so we can work with it
                ' easy AND, we can do Export Shape after that.

                ' First let's ReDim the number of Frames we have here.
                ' The intention is to have all of the shapes in memory.
                ReDim HDR_Frame(num_Frames - 1)

                ' Let's get every Frame.
                For i = 0 To (num_Frames - 1)
                    ' For Each Frame, Seek to Offset, and read DataFrame.
                    fileStream.Seek(shape_offset + HDR_Shapes.a_TBL_HDR_SHP(i).frame_Offset, SeekOrigin.Begin)

                    ' Then Put DataFrame in Struct
                    ReadUniqueFrame(i, sel_Shape, fileStream, readFLX)
                Next
            End If

            ' After reading Shape we close file.
            fileStream.Close()
        Catch
            MsgBox("Failed reading Frames from file.")
        End Try
    End Sub

    Public Sub OpenFLX()
        Dim x As Long

        ' Working with file
        Dim fileStream As New FileStream(Filename, FileMode.Open)
        Dim readFLX As BinaryReader = New BinaryReader(fileStream)

        Try
            ReDim HDR_FLX.hdr_1A_82(81)
            ReDim HDR_FLX.hdr_zero_2(1)
            ReDim HDR_FLX.hdr_zero_28(27)

            ' Let's read FLX Header to get some info, like number of Shapes.
            With HDR_FLX
                .hdr_1A_82 = readFLX.ReadBytes(82)
                .hdr_zero_2 = readFLX.ReadBytes(2)
                .hdr_nclss = readFLX.ReadUInt32
                .hdr_v = readFLX.ReadUInt32
                .hdr_fs = readFLX.ReadUInt32
                .hdr_crc = readFLX.ReadUInt32
                .hdr_zero_28 = readFLX.ReadBytes(28)
            End With

            ' Assign number of Shapes to var.
            num_Shapes = HDR_FLX.hdr_nclss

            'Redim the array
            ReDim FLX_Table_Shapes(num_Shapes - 1)

            ' Let's get Table of Shapes of FLX File for use it next time.
            ' We will put here two counters:
            ' Relative_FrameNumber = Will say the real shape number in FLX. (counts 0 shapes, begins by 0)
            ' Absolute_FrameNumber = Will say the frame number that has data in FLX. (do not counts 0 shapes, begins by 1)
            num_Shapes_Abs = 0

            For x = 0 To num_Shapes - 1
                With FLX_Table_Shapes(x)
                    ' Offset to the Shape
                    .Offset = readFLX.ReadUInt32

                    ' Size of the Shape (with all of its frames)
                    .Size = readFLX.ReadUInt32

                    ' Frame Counters
                    .Relative_FrameNumber = x
                    If .Size > 0 Then
                        num_Shapes_Abs = num_Shapes_Abs + 1
                        .Absolute_FrameNumber = num_Shapes_Abs
                    End If

                End With
            Next

            ' Mark as loaded
            FileLoaded = True

            ' We've done with fileStream
            readFLX.Close()
        Catch
            MsgBox("Failed to open selected .FLX file.")
        End Try
    End Sub

    Public Sub Load_PAL(ByVal filePAL As String)
        Dim i As Long, b_CRed As Byte, b_CGreen As Byte, b_CBlue

        ' Working with file
        Dim fileStream As New FileStream(filePAL, FileMode.Open)
        Dim readPAL As BinaryReader = New BinaryReader(fileStream)

        Try
            ' We do not need the first 4 bytes.
            fileStream.Seek(4, SeekOrigin.Begin)

            ' Get the 256 Colour Palette.
            For i = 0 To 254
                ' Each Bye has a value between 0 and 63, which is equal to 0 to 255.
                b_CRed = readPAL.ReadByte
                b_CGreen = readPAL.ReadByte
                b_CBlue = readPAL.ReadByte

                s_RGBPAL(i) = Color.FromArgb(255, (b_CRed / 63) * 255, (b_CGreen / 63) * 255, (b_CBlue / 63) * 255)
            Next i

            ' The index 255 of U8PAL is transparent. We do it apart.
            b_CRed = readPAL.ReadByte
            b_CGreen = readPAL.ReadByte
            b_CBlue = readPAL.ReadByte

            ' Put color in own Palette...
            s_RGBPAL(255) = Color.FromArgb(255, (b_CRed / 63) * 255, (b_CGreen / 63) * 255, (b_CBlue / 63) * 255)

            ' Finished. We close opened file.
            fileStream.Close()

            ' Put to True var to have been loaded PAL file.
            PALLoaded = True

            ' Put text into TextBox.
            txtPAL.Text = "U8PAL.PAL"
        Catch
            MsgBox("Failed to open Utima 8 .PAL file.")
        End Try
    End Sub

    Private Sub Form1_Shown(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Shown
        ' Ok, let's make visible PAL Form if U8PAL.PAL has been loaded.
        If PALLoaded Then
            frmPAL.Show()
            frmPAL.BringToFront()
        End If
    End Sub

    Private Sub Form1_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        ' Get Path where is executing U8FX.
        Path = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().CodeBase)
        Path = New Uri(Path).LocalPath

        ' Load Palette from file by Default if exists in Directory.
        If (My.Computer.FileSystem.FileExists(Path + "\U8PAL.PAL")) Then
            Load_PAL(Path + "\U8PAL.PAL")
        Else
            ' If no PAL file, we show a message.
            MsgBox("You should open U8PAL.PAL Palette file first.")
        End If

        ' Assign inital pbFrame (picture box location) to center image in window
        ' when drawing shapes
        x_loc_ori = pbFrame.Location.X
        y_loc_ori = pbFrame.Location.Y

        ' Init some special vars.
        Frame_Imported = False
        Exporting_Frame = False
        Shape_Loaded = False

        Me.Show()
    End Sub

    Private Sub Open_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnOpen.Click
        Dim result As Integer

        If Shape_Loaded Then
            result = MessageBox.Show("Ya have Loaded a Shape." & vbNewLine + _
                                     "Do you really want to open a FLEX file?", "Warning", MessageBoxButtons.OKCancel)
        End If

        If result = vbCancel Then
            Exit Sub
        Else
            Shape_Loaded = False
        End If

        ' We check if there is any PAL file readed.
        If txtPAL.Text.Length > 0 Then
            ' Set Directory in txtPath TextBox.
            OpenFLXDialog.InitialDirectory = Path

            ' Set Filter for U8 FLX Format (files).
            OpenFLXDialog.Filter = "Ultima 8 Graphics file (FLX)|*.flx|All files (*.*)|*.*"

            ' Show Dialog for Open files.
            If (OpenFLXDialog.ShowDialog() = DialogResult.OK) Then
                If My.Computer.FileSystem.FileExists(OpenFLXDialog.FileName) Then
                    ' Set selected Filename to txtPath TextBox.
                    txtPath.Text = OpenFLXDialog.FileName

                    ' Call OpenFLX to open FLX file selected.
                    ' The returned value gives the number of Shapes.
                    Filename = txtPath.Text
                    OpenFLX()

                    ' Let's know what we've readed (U8FONTS.FLX, U8GUMPS.FLX or U8SHAPES.FLX).
                    ' Shapes U8FONTS = &H15
                    '        U8GUMPs = &H599
                    If num_Shapes = 16 Then
                        FileLoadedU8FONTS = True
                        FileLoadedU8GUMPS = False
                        lbShapeType.Text = "FONT"
                    ElseIf num_Shapes = 256 Then
                        FileLoadedU8FONTS = False
                        FileLoadedU8GUMPS = True
                        lbShapeType.Text = "GUMP"
                    Else
                        FileLoadedU8FONTS = False
                        FileLoadedU8GUMPS = False
                        lbShapeType.Text = "SHAPE"
                    End If


                    ' Enable some things in Tool
                    cbAZ.Enabled = True
                    cbTransparency.Enabled = True
                    tbZoom.Enabled = True
                    tbZoom.Value = 1

                    btnEF.Enabled = True
                    btnIF.Enabled = True
                    btnSS.Enabled = True
                    btnEAF.Enabled = True

                    nudShape.Enabled = True
                    nudShape.Minimum = 1
                    nudShape.Maximum = num_Shapes_Abs
                    nudShape.Value = 1

                    nudShape_ValueChanged(sender, e)

                    lbShapes.Text = "/ " + (num_Shapes_Abs).ToString

                    Shape_Loaded = False

                Else
                    ' There is no FLX file.
                    MsgBox("Failed opening .FLX file.")
                End If
            End If
        Else
            ' If no PAL file, we show a message.
            MsgBox("You should open U8PAL.PAL Palette file first.")
        End If
    End Sub

    Private Sub OpenPAL_FileOk(ByVal sender As System.Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles OpenPALDialog.FileOk
        ' Load Palette from selected file.
        Load_PAL(OpenPALDialog.FileName)
    End Sub

    Private Sub btnExit_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnExit.Click
        Me.Close()
    End Sub

    Private Sub btnPAL_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnPAL.Click
        If PALLoaded Then
            frmPAL.Visible = True
            frmPAL.BringToFront()
        Else
            ' Set Directory in txtPath TextBox.
            OpenPALDialog.InitialDirectory = Path
            OpenPALDialog.FileName = Path + "\U8PAL.PAL"

            ' Set Filter for open Ultima VIII Palette (PAL) file.
            OpenPALDialog.Filter = "Ultima 8 U8PAL.PAL file|*.pal|All files (*.*)|*.*"

            ' Show Dialog.
            If (OpenPALDialog.ShowDialog() = DialogResult.OK) Then
                If (txtPAL.Text <> "") Then
                    ' Set ONLY name of the PAL file. Only for show purposes.
                    txtPAL.Text = OpenPALDialog.SafeFileName

                    ' Shows Palette in PAL Form.
                    PALLoaded = True
                    frmPAL.Show()
                Else
                    MsgBox("U8 Palette not selected.")
                End If
            End If
        End If
    End Sub

    Private Sub nudShape_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles nudShape.ValueChanged
        ' Read FLX file, locate selected SHAPE and get number of frames to put in NumericUpDown.
        ' Then Select first Frame in NumericUpDown to Draw first Image.

        ' First we search for the Absolute Shape
        Dim pos As Long
        Dim result As Integer
        result = DialogResult.OK


        If Frame_Imported Then
            result = MessageBox.Show("You have imported a Frame without saving it." & vbNewLine + _
                                     "Do you really want to change the Shape?", "Warning", MessageBoxButtons.OKCancel)
        End If

        If result = DialogResult.OK Then
            If FileLoaded Then
                pos = GetRealShape(nudShape.Value)
                ' Ok, read Relative Shape
                ReadShape(pos)
            End If

            If num_Frames > 0 Then
                nudFrame.Enabled = True
                nudFrame.Minimum = 1
                nudFrame.Maximum = num_Frames
                lbFrames.Text = "/ " + (num_Frames).ToString

                nudFrame.Value = nudFrame.Minimum

                ' Set the first Frame
                nudFrame_ValueChanged(sender, e)

                If FileLoaded Then
                    lbGS.Text = HDR_Shapes.MaxWidth.ToString + " x " + HDR_Shapes.MaxHeight.ToString

                    If Shape_Loaded Then
                        lbSO.Text = 0
                        lbSZ.Text = ShapeSize
                    Else
                        lbSO.Text = FLX_Table_Shapes(pos).Offset.ToString
                        lbSZ.Text = FLX_Table_Shapes(pos).Size.ToString
                    End If
                End If
            Else
                nudFrame.Minimum = 1
                nudFrame.Maximum = 1
                lbFrames.Text = "/ 1"
                nudFrame.Enabled = False
            End If

            Frame_Imported = False
        End If
    End Sub

    Private Sub nudFrame_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles nudFrame.ValueChanged
        ' When Frame Selected, we draw it
        If num_Frames > 0 Then
            DrawFrame(pbFrame, nudFrame.Value - 1)

            If FileLoaded Then
                lbCF.Text = HDR_Frame(nudFrame.Value - 1).frm_Width.ToString + " x " + HDR_Frame(nudFrame.Value - 1).frm_Height.ToString

                lbFOS.Text = HDR_Shapes.a_TBL_HDR_SHP(nudFrame.Value - 1).frame_Offset.ToString()
                lbFS.Text = HDR_Shapes.a_TBL_HDR_SHP(nudFrame.Value - 1).frame_Size.ToString

                If HDR_Frame(nudFrame.Value - 1).Compression = 1 Then
                    cbCompressed.Checked = True
                Else
                    cbCompressed.Checked = False
                End If
            End If
        End If
    End Sub

    Private Sub cbTransparency_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cbTransparency.CheckedChanged
        nudFrame_ValueChanged(sender, e)
    End Sub

    Private Sub pbFrame_MouseDown(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles pbFrame.MouseDown
        If FileLoaded Then
            frmPAL.SetInfoColor(pbFrame, e.X, e.Y)
            frmPAL.BringToFront()
        End If
    End Sub

    Private Sub pbFrame_MouseEnter(ByVal sender As Object, ByVal e As System.EventArgs) Handles pbFrame.MouseEnter
        If FileLoaded Then
            Me.Cursor = Cursors.Cross
        End If
    End Sub

    Private Sub pbFrame_MouseLeave(ByVal sender As Object, ByVal e As System.EventArgs) Handles pbFrame.MouseLeave
        If FileLoaded Then
            Me.Cursor = Cursors.Default
        End If
    End Sub

    Private Sub tbZoom_Scroll(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tbZoom.Scroll
        nudFrame_ValueChanged(sender, e)
    End Sub

    Private Sub cbAZ_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cbAZ.CheckedChanged
        If cbAZ.Checked Then
            tbZoom.Enabled = False
        Else
            tbZoom.Enabled = True
        End If

        nudFrame_ValueChanged(sender, e)
    End Sub

    Private Sub btnEF_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnEF.Click
        Dim n_shp As Integer, n_frm As Integer

        n_shp = GetRealShape(nudShape.Value)
        n_frm = nudFrame.Value - 1

        ' Set Directory in txtPath TextBox.
        ExpFRMDialog.InitialDirectory = Path
        ExpFRMDialog.FileName = Path + "\SHP" + n_shp.ToString("D4") + "_FRM" + n_frm.ToString("D4") + ".png"

        ' Set Filter for open Ultima VIII Palette (PAL) file.
        ExpFRMDialog.Filter = ".PNG file|*.png|All files (*.*)|*.*"

        ' Show Dialog.
        Try
            If (ExpFRMDialog.ShowDialog() = DialogResult.OK) Then
                If ExpFRMDialog.FileName <> "" Then
                    ' Only export original image, with zoom = 1 and original Color for Transparency.
                    'cbTransparency.Checked = False
                    'cbAZ.Checked = False
                    'tbZoom.Value = 1
                    'nudFrame_ValueChanged(sender, e)

                    ' Let know to drawframe that it comes from an export frame
                    Exporting_Frame = True
                    DrawFrame(pbFrame_invisible, nudFrame.Value - 1)

                    Dim BMP As New Bitmap(pbFrame_invisible.Image)

                    BMP.Save(ExpFRMDialog.FileName, System.Drawing.Imaging.ImageFormat.Png)

                    BMP.Dispose()
                    Exporting_Frame = False

                    MsgBox("Exporting Frame... DONE!")
                Else
                    MsgBox("File not selected.")
                End If
            End If
        Catch
            MsgBox("Error exporting .PNG file.")
        End Try

    End Sub

    Public Function IsTransparent(ByRef cColor As Color) As Boolean
        ' Ok, locate which color is.
        ' If the index in PAL is 255 then it is a Transparency
        If frmPAL.GetColor(cColor) = 255 Then
            Return True
        Else
            Return False
        End If
    End Function

    Public Sub Update_Shape(ByRef num_frame As Long, ByRef num_frame_size As Long)
        'original_fsize = HDR_Shapes.a_TBL_HDR_SHP(num_frame).frame_Size
        HDR_Shapes.a_TBL_HDR_SHP(num_frame).frame_Size = num_frame_size

        ' Modify Offsets for each frame from (num_frame+1) to (num_Frames - 1)
        ' with frame_Size = frame_Size - original_fsize + num_frame_size
        Dim i As Integer
        For i = num_frame + 1 To num_Frames - 1
            'original_fsize = HDR_Shapes.a_TBL_HDR_SHP(i).frame_Size

            HDR_Shapes.a_TBL_HDR_SHP(i).frame_realOffset = HDR_Shapes.a_TBL_HDR_SHP(i - 1).frame_realOffset + _
                                                           HDR_Shapes.a_TBL_HDR_SHP(i - 1).frame_Size
            If FileLoadedU8GUMPS Then
                HDR_Shapes.a_TBL_HDR_SHP(i).frame_realOffset = HDR_Shapes.a_TBL_HDR_SHP(i).frame_realOffset + 8
            End If
        Next i
    End Sub

    Private Sub ConvertPNG2U8Frame(ByRef BMP As Bitmap)
        ' We need to put the data on a new DATAIMAGE var to make calculations.
        Dim new_HDRFrame As s_FRAME
        Dim NEWDATAIMAGE() As Byte
        Dim l_pos_NDI As Long, l_pos_NDI_save As Long, l_pos_NDI_line As Long
        Dim x As Long, y As Long, num_act_frame As Long, num_table_line As Long

        ' We only do all this it if the Width and Height are less or equal to that of the
        ' shape, if not, we print a message.
        If BMP.Width > HDR_Shapes.MaxWidth Or BMP.Height > HDR_Shapes.MaxHeight Then
            MsgBox("The frame has a Width and/or Height greater than the max sizes of the Shape.")
            Exit Sub
        End If

        ' Init some vars
        l_pos_NDI = 0 : l_pos_NDI_line = 0 : l_pos_NDI_save = 0 : x = 0 : y = 0
        num_act_frame = nudFrame.Value - 1
        num_table_line = 0
        new_HDRFrame = Nothing
        NEWDATAIMAGE = Nothing

        ' Let's prepare somehow the Frame HDR.
        ' Basic data from the actual Frame
        new_HDRFrame.Type = HDR_Frame(num_act_frame).Type
        new_HDRFrame.FrameNumber = HDR_Frame(num_act_frame).FrameNumber
        new_HDRFrame.Unknown = HDR_Frame(num_act_frame).Unknown

        ' New data of the frame.
        new_HDRFrame.frm_Width = BMP.Width
        new_HDRFrame.frm_Height = BMP.Height

        ' The Offset would be the same as previous one.
        ' No compression (we only are going to do no compressed images.
        new_HDRFrame.x_Offset = HDR_Frame(num_act_frame).x_Offset
        new_HDRFrame.y_Offset = HDR_Frame(num_act_frame).y_Offset
        new_HDRFrame.Compression = 0

        ' We know the number of lines (the Heigth of the new image).
        ' Lets Redim table_lines
        ReDim new_HDRFrame.table_Lines(BMP.Height - 1)

        ' First table to first line offset is calculated as
        ' Frame Height * 2
        ' After that we need to calculate.
        new_HDRFrame.table_Lines(num_table_line) = new_HDRFrame.frm_Height * 2

        ' For each line, we will make NEWDATAIMAGE
        ' We will need to get also the offsets of each line (a counter).
        Dim x_pixel_counter As Integer
        Dim x_data_len As Long
        Dim actual_Color As Color, old_color As Color

        ' Ok, let's do each line until pixel_counter is the same as BMP.Width
        ' and THAT for each line
        x_data_len = 0 : x_pixel_counter = 0

        ' Prepare colors to compare
        actual_Color = BMP.GetPixel(x_pixel_counter, num_table_line)
        old_color = actual_Color

        Do While num_table_line < BMP.Height
            Do While x_pixel_counter < BMP.Width
                ' First check what color is this pixel
                ' If is color, prepare DATAIMAGE for read colors.
                ' If not (transparent), put data_len of x_position for transparent color
                If IsTransparent(actual_Color) Then
                    x_pixel_counter = x_pixel_counter + 1
                    x_data_len = x_data_len + 1
                    old_color = actual_Color
                    If x_pixel_counter < BMP.Width Then
                        actual_Color = BMP.GetPixel(x_pixel_counter, num_table_line)
                    End If
                Else
                    If x_data_len > 0 Then
                        ' We readed Transparent color until now.
                        ReDim Preserve NEWDATAIMAGE(l_pos_NDI)
                        NEWDATAIMAGE(l_pos_NDI) = x_data_len
                        l_pos_NDI = l_pos_NDI + 1
                        l_pos_NDI_line = l_pos_NDI_line + 1

                        ' Reset reading of pixels
                        x_data_len = 0
                    Else
                        ' We found some not transparent color
                        ' BUT: a) b)
                        ' a) We need to put the x pos of beginning to draw
                        If l_pos_NDI_line = 0 Then
                            ReDim Preserve NEWDATAIMAGE(l_pos_NDI)
                            NEWDATAIMAGE(l_pos_NDI) = 0
                            l_pos_NDI = l_pos_NDI + 1
                            l_pos_NDI_line = l_pos_NDI_line + 1
                        End If

                        ' b) Prepare length of pixels drawn because
                        ' we don't know how many draw.
                        l_pos_NDI_save = l_pos_NDI
                        ReDim Preserve NEWDATAIMAGE(l_pos_NDI)
                        NEWDATAIMAGE(l_pos_NDI_save) = 0
                        l_pos_NDI = l_pos_NDI + 1
                        l_pos_NDI_line = l_pos_NDI_line + 1

                        ' Write each non transparent color individually
                        ' while not transparent new colors or until Line Width
                        While Not IsTransparent(actual_Color) And x_pixel_counter < BMP.Width
                            ReDim Preserve NEWDATAIMAGE(l_pos_NDI)
                            NEWDATAIMAGE(l_pos_NDI) = frmPAL.GetColor(actual_Color)
                            l_pos_NDI = l_pos_NDI + 1
                            l_pos_NDI_line = l_pos_NDI_line + 1

                            ' Update counter
                            NEWDATAIMAGE(l_pos_NDI_save) = NEWDATAIMAGE(l_pos_NDI_save) + 1

                            ' Get new data
                            x_pixel_counter = x_pixel_counter + 1
                            old_color = actual_Color
                            If x_pixel_counter < BMP.Width Then
                                actual_Color = BMP.GetPixel(x_pixel_counter, num_table_line)
                            End If
                        End While
                    End If
                End If

                ' Case where the line is all transparent
                If x_data_len = BMP.Width Then
                    ' We readed Transparent color until now.
                    ReDim Preserve NEWDATAIMAGE(l_pos_NDI)
                    NEWDATAIMAGE(l_pos_NDI) = x_data_len
                    l_pos_NDI = l_pos_NDI + 1
                    l_pos_NDI_line = l_pos_NDI_line + 1
                End If
            Loop

            ' Let's check last data. Maybe we have here at the end
            ' some transparent colors.
            If x_data_len > 0 Then
                ReDim Preserve NEWDATAIMAGE(l_pos_NDI)
                NEWDATAIMAGE(l_pos_NDI) = x_data_len
                l_pos_NDI = l_pos_NDI + 1
                l_pos_NDI_line = l_pos_NDI_line + 1
            End If

            ' Next line
            num_table_line = num_table_line + 1

            ' Reset x pos in image counter for begin new line
            x_pixel_counter = 0
            x_data_len = 0

            ' Update table_line and Prepare colors to compare
            If num_table_line < BMP.Height Then
                ' Update table_Line(num_table_line)
                new_HDRFrame.table_Lines(num_table_line) = (new_HDRFrame.table_Lines(num_table_line - 1) - 2) + (l_pos_NDI_line)

                actual_Color = BMP.GetPixel(x_pixel_counter, num_table_line)
                old_color = actual_Color
                l_pos_NDI_line = 0
            End If
        Loop

        ' Once we have DATAIMAGE, we need to Update HDR_Frame original values
        ' DATAIMAGE and table_lines
        HDR_Frame(num_act_frame) = new_HDRFrame
        HDR_Frame(num_act_frame).DATAIMAGE = NEWDATAIMAGE

        ' We will update also:
        ' a) HDR_Shapes(num_shapes).Size (adding or substracting this frame size).
        ' b) HDR_Shapes(actual_shape).Offset to HDR_Shapes(num_shapes-1).Offset with new frame size.

        ' First, let's see how many bytes is this frame.
        Dim new_frame_size As Long
        If FileLoadedU8FONTS Then
            ' Formula: HDR_SIZE (18bytes) + Offsets to Lines (TABLE_LINES * 2) + NEWDATAIMAGE
            new_frame_size = 18 + (num_table_line * 2) + (l_pos_NDI)
        ElseIf FileLoadedU8GUMPS Then
            ' Somehow first eight bytes (eight 0 -zeroes- are not counted) for GUMPS.
            new_frame_size = 10 + (num_table_line * 2) + (l_pos_NDI)
        Else
            ' I will not try but SHAPES has the real counting.
            new_frame_size = (num_table_line * 2) + (l_pos_NDI)
        End If
        Update_Shape(num_act_frame, new_frame_size)

        ' We have now Shapes prepared for writing.
    End Sub

    Private Sub btnIF_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnIF.Click
        ' Set Directory in txtPath TextBox.
        ImpFRMDialog.InitialDirectory = Path
        ImpFRMDialog.FileName = ""

        ' Set Filter for open Ultima VIII Palette (PAL) file.
        ImpFRMDialog.Filter = ".PNG file|*.png|All files (*.*)|*.*"

        Try
            ' Show Dialog.
            If (ImpFRMDialog.ShowDialog() = DialogResult.OK) Then
                If ImpFRMDialog.FileName <> "" Then
                    Dim BMP As New Bitmap(ImpFRMDialog.FileName)

                    ' Ok, Here we need to Call the Convert2U8 process.
                    ConvertPNG2U8Frame(BMP)

                    BMP.Dispose()

                    nudFrame_ValueChanged(sender, e)

                    Frame_Imported = True
                Else
                    MsgBox("File not selected.")
                End If
            End If
        Catch
            MsgBox("Error importing .PNG file.")
        End Try
    End Sub

    Public Sub SaveShape(ByVal writer As BinaryWriter, ByRef num_Shape As Long)
        Dim n_frms As Long
        ' Let's mount a .SHP file with the Frames we have in memory.
        ' Steps:
        ' a) Write HDR_SHP
        ' b) Write Table of Frames
        ' c) Write Each Frame

        Try
            ' Write Header
            writer.Write(HDR_Shapes.MaxWidth)
            writer.Write(HDR_Shapes.MaxHeight)
            writer.Write(HDR_Shapes.NumberofFrames)

            ' Write Table of Frames
            For n_frms = 0 To num_Frames - 1
                writer.Write(HDR_Shapes.a_TBL_HDR_SHP(n_frms).frame_realOffset)
                writer.Write(HDR_Shapes.a_TBL_HDR_SHP(n_frms).frame_NumSubSHP)
                writer.Write(HDR_Shapes.a_TBL_HDR_SHP(n_frms).frame_MultiSHP)
                writer.Write(HDR_Shapes.a_TBL_HDR_SHP(n_frms).frame_Size)
            Next

            ' Write Each Frame
            For n_frms = 0 To num_Frames - 1
                With HDR_Frame(n_frms)
                    writer.Write(.Type)
                    writer.Write(.FrameNumber)
                    writer.Write(.Unknown)
                    writer.Write(.Compression)
                    writer.Write(.frm_Width)
                    writer.Write(.frm_Height)
                    writer.Write(.x_Offset)
                    writer.Write(.y_Offset)

                    ' Write each Table of Lines
                    Dim line As Integer
                    For line = 0 To .frm_Height - 1
                        writer.Write(.table_Lines(line))
                    Next line

                    ' Write DATAIMAGE
                    writer.Write(.DATAIMAGE)
                End With
            Next
            ' Done. Shape Saved.
        Catch
            MsgBox("Error writing data to .SHP file.")
        End Try
    End Sub

    Private Sub btnSS_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSS.Click
        Dim n_shp As Integer

        n_shp = GetRealShape(nudShape.Value)

        ' Set Directory in txtPath TextBox.
        SaveSHPDialog.InitialDirectory = Path
        SaveSHPDialog.FileName = Path + "\" + n_shp.ToString("X4") + ".shp"

        ' Set Filter for open Ultima VIII Palette (PAL) file.
        SaveSHPDialog.Filter = ".SHP file|*.shp|All files (*.*)|*.*"

        Try
            ' Show Dialog.
            If SaveSHPDialog.ShowDialog() = DialogResult.OK Then
                If SaveSHPDialog.FileName <> "" Then
                    ' Prepare file for write
                    Using writer As BinaryWriter = New BinaryWriter(File.Open(SaveSHPDialog.FileName, FileMode.Create))
                        ' Call SaveShape function
                        SaveShape(writer, n_shp)

                        Frame_Imported = False
                    End Using

                    MsgBox("Export of .SHP file... DONE!")
                Else
                    MsgBox("File not selected.")
                End If
            End If
        Catch
            MsgBox("Error saving .SHP Shape file.")
        End Try
    End Sub

    Private Sub btnEAF_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnEAF.Click
        ' Well, for each Shape, we need to:
        ' a) Prepare Directory
        ' b) Select shape, and for each shape (real shape), extract it.
        Dim n_shp As Integer, n_frm As Integer, n_frm_old As Integer
        Dim FileName As String

        n_shp = GetRealShape(nudShape.Value)
        n_frm_old = nudFrame.Value : n_frm = 0

        ' Set Directory in txtPath TextBox.
        ExpSFDFDialog.SelectedPath = Path

        ' Show Dialog.
        Try
            If (ExpSFDFDialog.ShowDialog() = DialogResult.OK) Then
                ' For each frame...
                ' Only export original image, with zoom = 1 and original Color for Transparency.
                If ExpSFDFDialog.SelectedPath <> "" Then

                    ' Let know to drawframe that it comes from an export frame
                    Exporting_Frame = True

                    For n_frm = 0 To HDR_Shapes.NumberofFrames - 1
                        FileName = ExpSFDFDialog.SelectedPath + "\SHP" + n_shp.ToString("D4") + "_FRM" + n_frm.ToString("D4") + ".png"

                        DrawFrame(pbFrame_invisible, n_frm)
                        Dim BMP As New Bitmap(pbFrame_invisible.Image)

                        BMP.Save(FileName, System.Drawing.Imaging.ImageFormat.Png)

                        BMP.Dispose()
                    Next n_frm
                    Exporting_Frame = False

                    MsgBox("Exporting ALL Frames of the Shape... DONE!")
                Else
                    MsgBox("Directory not selected.")
                End If
            End If
        Catch
            MsgBox("Error exporting .PNG files of the shape.")
        End Try
    End Sub


    Private Sub btnLS_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnLS.Click
        Dim n_shp As Integer
        Dim result As Integer

        If Frame_Imported Then
            result = MessageBox.Show("You have imported a Frame without saving it." & vbNewLine + _
                                     "Do you really want to change the Shape?", "Warning", MessageBoxButtons.OKCancel)
        End If

        If result = vbCancel Then Exit Sub

        n_shp = 1

        ' Set Directory in txtPath TextBox.
        LoadSHPDialog.InitialDirectory = Path
        LoadSHPDialog.FileName = ""

        ' Set Filter for open Ultima VIII Palette (PAL) file.
        LoadSHPDialog.Filter = ".SHP file|*.shp|All files (*.*)|*.*"

        Try
            ' Show Dialog.
            If LoadSHPDialog.ShowDialog() = DialogResult.OK Then
                If LoadSHPDialog.FileName <> "" Then
                    ' Let's put 1 in shape only for visible purposes.
                    n_shp = 0
                    Shape_Loaded = True
                    FileLoaded = True

                    ' And disable some objects.
                    nudShape.Enabled = False

                    ' Prepare file to load.
                    Filename = LoadSHPDialog.FileName
                    ShapeSize = My.Computer.FileSystem.GetFileInfo(Filename).Length

                    ' We need to know what type of shape we are loading (FONTS, GUMPS or SHAPES)
                    ' Well, let's ask to the user.... I have not know any other method.
                    frmSelShape.ShowDialog()

                    If ShapeType = 1 Then
                        FileLoadedU8FONTS = True
                        FileLoadedU8GUMPS = False
                        lbShapeType.Text = "FONT"
                    ElseIf ShapeType = 2 Then
                        FileLoadedU8FONTS = False
                        FileLoadedU8GUMPS = True
                        lbShapeType.Text = "GUMP"
                    ElseIf ShapeType = 3 Then
                        FileLoadedU8FONTS = False
                        FileLoadedU8GUMPS = False
                        lbShapeType.Text = "SHAPE"
                    End If

                    If ShapeType > 0 Then
                        ' We use the ReadShape routine.
                        ReadShape(n_shp)

                        ' Print it
                        nudShape_ValueChanged(sender, e)

                        ' Enable some things in Tool
                        cbAZ.Enabled = True
                        cbTransparency.Enabled = True
                        tbZoom.Enabled = True
                        tbZoom.Value = 1

                        btnEF.Enabled = True
                        btnIF.Enabled = True
                        btnSS.Enabled = True
                        btnEAF.Enabled = True

                        Frame_Imported = False

                        txtPath.Text = LoadSHPDialog.FileName
                    End If
                Else
                    MsgBox("File not selected.")
                End If
            End If
        Catch
            MsgBox("Error loading .SHP shape file.")
        End Try
    End Sub
End Class
