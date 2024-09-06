(Dialog params_dialog_lcr
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
        (CommitCancel.Label "Cancel")
        (CommitCancel.TopOffset 10)
        (CommitCancel.BottomOffset 10)
        (CommitCancel.LeftOffset 8)
        (CommitCancel.RightOffset 10)
        (CommitCancel.ResourceHints "Template:OKCancelDialog.CommitCancel")
        (.TopOffset 0)
        (.BottomOffset 0)
        (.LeftOffset 0)
        (.RightOffset 0)
        (.DefaultButton "CommitOK")
        (.ResourceHints "Template:OKCancelDialog" "Guidelines:Creo4-Dialog" "GuidelinesVersion:2" "Version:Creo4" "@Subgrid0x2.Template:OKCancelDialog.CommitBar")
        (.Accelerator "Esc")
        (.Label "Parameters")
        (.AttachLeft True)
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
        (SubLayout Layout1)
    )
    (Resources
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
                Layout1
            )
        )
    )
)
(Layout Layout1
    (Components
        (SubLayout dimension_layout)
        (SubLayout desc_layout)
    )
    (Resources
        (.AttachLeft True)
        (.AttachRight True)
        (.AttachTop True)
        (.AttachBottom True)
        (.ResourceHints "UserOverrideAttrs:TopOffset")
        (.TopOffset 3)
        (.BottomOffset 0)
        (.LeftOffset 0)
        (.RightOffset 0)
        (.Layout
            (Grid
                (Rows 0 1 1)
                (Cols 1)
                dimension_layout
                (Pos 3 1)
                desc_layout
            )
        )
    )
)
(Layout dimension_layout
    (Components
        (Label height_lbl)
        (InputPanel fanout_height_ip)
        (Label height_units)
        (Label circle_radius_lbl)
        (InputPanel circle_radius_ip)
        (Label width_units)
        (Label bondpad_side_lbl)
        (InputPanel bondpad_side_ip)
        (Label angle_units)
    )
    (Resources
        (height_lbl.Label "Fanout Height:")
        (height_lbl.AttachLeft True)
        (height_lbl.ResourceHints "UserOverrideAttrs:TopOffset,BottomOffset")
        (height_lbl.Alignment 0)
        (height_lbl.Resizeable True)
        (height_lbl.TopOffset 10)
        (height_lbl.BottomOffset 10)
        (height_lbl.LeftOffset 7)
        (height_lbl.RightOffset 4)
        (fanout_height_ip.Columns 5)
        (fanout_height_ip.ResourceHints "UserOverrideAttrs:LeftOffset")
        (fanout_height_ip.TopOffset 6)
        (fanout_height_ip.BottomOffset 3)
        (fanout_height_ip.LeftOffset 40)
        (fanout_height_ip.RightOffset 4)
        (fanout_height_ip.InputType 3)
        (fanout_height_ip.Digits 2)
        (height_units.Label "micron")
        (height_units.AttachLeft True)
        (height_units.Alignment 0)
        (height_units.Resizeable True)
        (height_units.TopOffset 6)
        (height_units.BottomOffset 3)
        (height_units.LeftOffset 3)
        (height_units.RightOffset 7)
        (circle_radius_lbl.Label "Circle Radius:")
        (circle_radius_lbl.AttachLeft True)
        (circle_radius_lbl.ResourceHints "UserOverrideAttrs:TopOffset,BottomOffset")
        (circle_radius_lbl.Alignment 0)
        (circle_radius_lbl.Resizeable True)
        (circle_radius_lbl.TopOffset 10)
        (circle_radius_lbl.BottomOffset 10)
        (circle_radius_lbl.LeftOffset 7)
        (circle_radius_lbl.RightOffset 4)
        (circle_radius_ip.Columns 5)
        (circle_radius_ip.ResourceHints "UserOverrideAttrs:LeftOffset")
        (circle_radius_ip.TopOffset 3)
        (circle_radius_ip.BottomOffset 3)
        (circle_radius_ip.LeftOffset 40)
        (circle_radius_ip.RightOffset 4)
        (circle_radius_ip.InputType 3)
        (circle_radius_ip.Digits 2)
        (width_units.Label "micron")
        (width_units.AttachLeft True)
        (width_units.Alignment 0)
        (width_units.Resizeable True)
        (width_units.TopOffset 3)
        (width_units.BottomOffset 3)
        (width_units.LeftOffset 3)
        (width_units.RightOffset 7)
        (bondpad_side_lbl.Label "Bondpad Side:")
        (bondpad_side_lbl.AttachLeft True)
        (bondpad_side_lbl.ResourceHints "UserOverrideAttrs:TopOffset,BottomOffset")
        (bondpad_side_lbl.Alignment 0)
        (bondpad_side_lbl.Resizeable True)
        (bondpad_side_lbl.TopOffset 10)
        (bondpad_side_lbl.BottomOffset 10)
        (bondpad_side_lbl.LeftOffset 7)
        (bondpad_side_lbl.RightOffset 4)
        (bondpad_side_ip.Columns 5)
        (bondpad_side_ip.ResourceHints "UserOverrideAttrs:LeftOffset")
        (bondpad_side_ip.TopOffset 3)
        (bondpad_side_ip.BottomOffset 6)
        (bondpad_side_ip.LeftOffset 40)
        (bondpad_side_ip.RightOffset 4)
        (bondpad_side_ip.InputType 3)
        (bondpad_side_ip.Digits 2)
        (angle_units.Label "micron")
        (angle_units.AttachLeft True)
        (angle_units.Alignment 0)
        (angle_units.Resizeable True)
        (angle_units.TopOffset 3)
        (angle_units.BottomOffset 6)
        (angle_units.LeftOffset 3)
        (angle_units.RightOffset 7)
        (.AttachLeft True)
        (.AttachRight True)
        (.AttachTop True)
        (.AttachBottom True)
        (.ResourceHints "UserOverrideAttrs:RightOffset")
        (.TopOffset 0)
        (.BottomOffset 6)
        (.LeftOffset 0)
        (.RightOffset 3)
        (.Label "Dimensions")
        (.Decorated 1)
        (.Layout
            (Grid
                (Rows 0 0 0)
                (Cols 0 1 0)
                height_lbl fanout_height_ip height_units circle_radius_lbl circle_radius_ip width_units bondpad_side_lbl bondpad_side_ip angle_units
            )
        )
    )
)
(Layout desc_layout
    (Components
        (Label Label1)
        (TextArea desc_textarea)
    )
    (Resources
        (Label1.Label "Note")
        (Label1.AttachLeft True)
        (Label1.AttachTop True)
        (Label1.Alignment 0)
        (Label1.Resizeable True)
        (Label1.TopOffset 6)
        (Label1.BottomOffset 6)
        (Label1.LeftOffset 7)
        (Label1.RightOffset 4)
        (desc_textarea.Wrap True)
        (desc_textarea.TopOffset 6)
        (desc_textarea.BottomOffset 6)
        (desc_textarea.LeftOffset 3)
        (desc_textarea.RightOffset 7)
        (desc_textarea.ScrollBarsWhenNeeded True)
        (.Decorated 1)
        (.AttachLeft True)
        (.AttachRight True)
        (.AttachTop True)
        (.AttachBottom True)
        (.TopOffset 6)
        (.BottomOffset 0)
        (.LeftOffset 0)
        (.RightOffset 0)
        (.Layout
            (Grid
                (Rows 1)
                (Cols 0 1)
                Label1 desc_textarea
            )
        )
    )
)
