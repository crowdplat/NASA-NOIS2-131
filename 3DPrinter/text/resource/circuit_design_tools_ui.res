(Dialog circuit_design_tools_ui
    (Components
        (SubLayout Content)
        (Separator CommitSeparator)
        (PushButton CommitOK)
        (PushButton CommitCancel)
    )
    (Resources
        (CommitSeparator.ResourceHints "Template:OKCancelDialog.CommitSeparator")
        (CommitOK.Label "Place")
        (CommitOK.TopOffset 10)
        (CommitOK.BottomOffset 10)
        (CommitOK.LeftOffset 40)
        (CommitOK.RightOffset 0)
        (CommitOK.ResourceHints "Template:OKCancelDialog.CommitOK" "UserOverrideAttrs:Label")
        (CommitOK.Visible False)
        (CommitCancel.Label "Close")
        (CommitCancel.TopOffset 10)
        (CommitCancel.BottomOffset 10)
        (CommitCancel.LeftOffset 8)
        (CommitCancel.RightOffset 10)
        (CommitCancel.ResourceHints "Template:OKCancelDialog.CommitCancel" "UserOverrideAttrs:Label")
        (.TopOffset 0)
        (.BottomOffset 0)
        (.LeftOffset 0)
        (.RightOffset 0)
        (.DefaultButton "CommitOK")
        (.ResourceHints "Template:OKCancelDialog" "Guidelines:Creo4-Dialog" "GuidelinesVersion:2" "Version:Creo4" "@Subgrid0x2.Template:OKCancelDialog.CommitBar")
        (.Accelerator "Esc")
        (.AttachLeft True)
        (.Label "Circuit Design Automation Tools")
        (.Columns 20)
        (.StartLocation 3)
        (.Gravity 3)
        (.Layout
            (Grid
                (Rows 1 0 0)
                (Cols 1)
                Content CommitSeparator
                (Grid
                    (Rows 1)
                    (Cols 1 0 0)
                    (Pos 1 2)
                    CommitOK
                    (Pos 1 3)
                    CommitCancel
                )
            )
        )
    )
)
(Layout Content
    (Components
        (Tab Tab1 template_layout functions_layout)
    )
    (Resources
        (Tab1.Decorated 1)
        (Tab1.TopOffset 0)
        (Tab1.BottomOffset 0)
        (Tab1.LeftOffset 0)
        (Tab1.RightOffset 0)
        (.AttachLeft True)
        (.AttachRight True)
        (.AttachTop True)
        (.TopOffset 16)
        (.BottomOffset 16)
        (.LeftOffset 10)
        (.RightOffset 10)
        (.ResourceHints "Template:OKCancelDialog.Content")
        (.AttachBottom True)
        (.Layout
            (Grid
                (Rows 1)
                (Cols 1)
                Tab1
            )
        )
    )
)
(Layout template_layout
    (Components
        (SubLayout buttons_layout)
    )
    (Resources
        (.Label "Templates")
        (.TopOffset 0)
        (.BottomOffset 0)
        (.LeftOffset 0)
        (.RightOffset 0)
        (.Layout
            (Grid
                (Rows 0)
                (Cols 0)
                buttons_layout
            )
        )
    )
)
(Layout buttons_layout
    (Components
        (PushButton pb_1)
        (PushButton pb_3)
        (PushButton pb_6)
        (PushButton pb_4)
        (PushButton pb_5)
        (PushButton pb_8)
        (PushButton pb_7)
        (PushButton pb_9)
        (PushButton pb_11)
        (PushButton pb_10)
    )
    (Resources
        (pb_1.Bitmap "bondpad.jpg")
        (pb_1.AttachLeft True)
        (pb_1.Resizeable True)
        (pb_1.TopOffset 0)
        (pb_1.BottomOffset 3)
        (pb_1.LeftOffset 0)
        (pb_1.RightOffset 4)
        (pb_1.ButtonAlignment 5531)
        (pb_1.Label "Bondpad")
        (pb_1.BackgroundColor 10)
        (pb_3.Bitmap "bondpad.jpg")
        (pb_3.AttachLeft True)
        (pb_3.Resizeable True)
        (pb_3.TopOffset 0)
        (pb_3.BottomOffset 3)
        (pb_3.LeftOffset 3)
        (pb_3.RightOffset 4)
        (pb_3.ButtonAlignment 5531)
        (pb_3.Label "Update Bondpad")
        (pb_6.Bitmap "interdigitated_capacitor.jpg")
        (pb_6.AttachLeft True)
        (pb_6.Resizeable True)
        (pb_6.TopOffset 0)
        (pb_6.BottomOffset 3)
        (pb_6.LeftOffset 3)
        (pb_6.RightOffset 0)
        (pb_6.ButtonAlignment 5531)
        (pb_6.Label "Interdigitated capacitor")
        (pb_6.Wrap True)
        (pb_4.Bitmap "inductor_loop.jpg")
        (pb_4.AttachLeft True)
        (pb_4.Resizeable True)
        (pb_4.TopOffset 3)
        (pb_4.BottomOffset 3)
        (pb_4.LeftOffset 0)
        (pb_4.RightOffset 4)
        (pb_4.ButtonAlignment 5531)
        (pb_4.Label "Inductor Loop")
        (pb_4.Wrap True)
        (pb_5.Bitmap "serpentine_resistor.jpg")
        (pb_5.AttachLeft True)
        (pb_5.Resizeable True)
        (pb_5.TopOffset 3)
        (pb_5.BottomOffset 3)
        (pb_5.LeftOffset 3)
        (pb_5.RightOffset 4)
        (pb_5.ButtonAlignment 5531)
        (pb_5.Label "Serpentine Resistor")
        (pb_5.Wrap True)
        (pb_8.Bitmap "bondpad_pair.jpg")
        (pb_8.AttachLeft True)
        (pb_8.Resizeable True)
        (pb_8.TopOffset 3)
        (pb_8.BottomOffset 3)
        (pb_8.LeftOffset 3)
        (pb_8.RightOffset 0)
        (pb_8.ButtonAlignment 5531)
        (pb_8.Label "Large bondpad")
        (pb_8.Wrap True)
        (pb_7.Bitmap "bondpad_pair.jpg")
        (pb_7.AttachLeft True)
        (pb_7.Resizeable True)
        (pb_7.TopOffset 3)
        (pb_7.BottomOffset 3)
        (pb_7.LeftOffset 0)
        (pb_7.RightOffset 4)
        (pb_7.ButtonAlignment 5531)
        (pb_7.Label "Bondpad pair")
        (pb_7.Wrap True)
        (pb_9.Bitmap "serpentine_resistor.jpg")
        (pb_9.AttachLeft True)
        (pb_9.Resizeable True)
        (pb_9.TopOffset 3)
        (pb_9.BottomOffset 3)
        (pb_9.LeftOffset 3)
        (pb_9.RightOffset 4)
        (pb_9.ButtonAlignment 5531)
        (pb_9.Label "Resistor network")
        (pb_9.Wrap True)
        (pb_11.Bitmap "serpentine_resistor.jpg")
        (pb_11.AttachLeft True)
        (pb_11.Resizeable True)
        (pb_11.TopOffset 3)
        (pb_11.BottomOffset 3)
        (pb_11.LeftOffset 3)
        (pb_11.RightOffset 0)
        (pb_11.ButtonAlignment 5531)
        (pb_11.Label "Resistor network 2 (Inv.)")
        (pb_11.Wrap True)
        (pb_10.Bitmap "serpentine_resistor.jpg")
        (pb_10.AttachLeft True)
        (pb_10.Resizeable True)
        (pb_10.TopOffset 3)
        (pb_10.BottomOffset 0)
        (pb_10.LeftOffset 0)
        (pb_10.RightOffset 4)
        (pb_10.ButtonAlignment 5531)
        (pb_10.Label "Resistor network 2")
        (pb_10.Wrap True)
        (.AttachLeft True)
        (.AttachRight True)
        (.AttachTop True)
        (.AttachBottom True)
        (.TopOffset 13)
        (.BottomOffset 13)
        (.LeftOffset 10)
        (.RightOffset 10)
        (.Layout
            (Grid
                (Rows 0 0 0 0)
                (Cols 0 0 0)
                pb_1 pb_3 pb_6 pb_4 pb_5 pb_8 pb_7 pb_9 pb_11 pb_10
            )
        )
    )
)
(Layout functions_layout
    (Components
        (SubLayout Layout3)
    )
    (Resources
        (.Label "Functions")
        (.TopOffset 0)
        (.BottomOffset 0)
        (.LeftOffset 0)
        (.RightOffset 0)
        (.Layout
            (Grid
                (Rows 0)
                (Cols 0)
                Layout3
            )
        )
    )
)
(Layout Layout3
    (Components
        (PushButton fanout_pb)
    )
    (Resources
        (fanout_pb.AttachLeft True)
        (fanout_pb.Resizeable True)
        (fanout_pb.TopOffset 0)
        (fanout_pb.BottomOffset 0)
        (fanout_pb.LeftOffset 0)
        (fanout_pb.RightOffset 0)
        (fanout_pb.ButtonAlignment 5531)
        (fanout_pb.Label "Fanout")
        (fanout_pb.Columns 7)
        (fanout_pb.Bitmap "fanout.png")
        (.AttachLeft True)
        (.AttachRight True)
        (.AttachTop True)
        (.AttachBottom True)
        (.TopOffset 13)
        (.BottomOffset 13)
        (.LeftOffset 10)
        (.RightOffset 10)
        (.Layout
            (Grid
                (Rows 0)
                (Cols 0)
                fanout_pb
            )
        )
    )
)
