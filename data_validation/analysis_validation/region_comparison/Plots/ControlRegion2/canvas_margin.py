import os,ROOT

def canvas_margin(c1, c1_up, c1_down):

  c1_up.SetTopMargin( 0.07 )
  c1_up.SetBottomMargin( 0.025 )
  c1_up.SetLeftMargin( 0.15 )
  c1_up.SetRightMargin( 0.032 )

  c1_down.SetTopMargin( 0.035 )
  c1_down.SetBottomMargin( 0.4 )
  c1_down.SetLeftMargin( 0.15 )
  c1_down.SetRightMargin( 0.032 )

  c1.SetTopMargin( 0.05 )
  c1.SetBottomMargin( 0.13 )
  c1.SetRightMargin( 0.05 )
  c1.SetLeftMargin( 0.16 )

  return c1, c1_up, c1_down

def hist_axis(hist, hist_compare):

  hist.SetTitle("")

  ##==== top plot
  hist.GetYaxis().SetLabelSize(0.05)
  hist.GetYaxis().SetTitleSize(0.070)
  hist.GetYaxis().SetTitleOffset(1.10)
  ##==== hide x-axis for top plot
  hist.GetXaxis().SetLabelSize(0)

  ##==== bottom plot
  hist_compare.SetTitle("")

  hist_compare.GetXaxis().SetLabelSize(0.13)
  hist_compare.GetXaxis().SetTitleSize(0.18)

  hist_compare.GetYaxis().SetLabelSize(0.13)
  hist_compare.GetYaxis().SetTitleSize(0.16)
  hist_compare.GetYaxis().SetTitleOffset(0.4)

  return hist, hist_compare
