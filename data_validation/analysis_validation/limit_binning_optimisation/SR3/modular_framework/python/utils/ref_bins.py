def get_sr3_ref_edges(flav, era, jet, is_ltcut):
    """
    is_ltcut = True  means met < threshold
    is_ltcut = False means met >= threshold
    """

    edges = [0.0]

    def add(*vals):
        edges.extend(vals)

    # ----------------------------------
    # MuMu
    # ----------------------------------
    if flav == "MuMu":

        if era == "2016preVFP":
            if jet == "LowJet":
                add(150, 175, 200, 300) if is_ltcut else add(175, 250)
            else:
                add(160, 180, 200, 225, 275) if is_ltcut else add(175, 200, 250)

        elif era == "2016postVFP":
            if jet == "LowJet":
                add(130, 150, 200, 300) if is_ltcut else add(175, 250)
            else:
                add(130, 150, 175, 200, 275) if is_ltcut else add(150, 175, 225)

        elif era == "2017":
            if jet == "LowJet":
                add(150, 200, 250, 300) if is_ltcut else add(200, 275)
            else:
                add(175, 200, 250, 300, 375) if is_ltcut else add(200, 275, 350)

        elif era == "2018":
            if jet == "LowJet":
                add(200, 250, 300, 375) if is_ltcut else add(200, 400)
            else:
                add(200, 225, 275, 325, 400) if is_ltcut else add(200, 350, 450)

    # ----------------------------------
    # EE
    # ----------------------------------
    elif flav == "EE":

        if era.startswith("2016"):
            if jet == "LowJet":
                add(200, 400, 600, 900) if is_ltcut else add(200, 500)
            else:
                add(300, 375, 450, 550, 700) if is_ltcut else add(250, 350, 400)

        elif era == "2017":
            if jet == "LowJet":
                add(200, 400, 600, 900) if is_ltcut else add(180, 500)
            else:
                add(300, 375, 450, 550, 700) if is_ltcut else add(300, 450, 550)

        elif era == "2018":
            if jet == "LowJet":
                add(200, 400, 600, 900) if is_ltcut else add(200, 600)
            else:
                add(300, 400, 500, 600, 800) if is_ltcut else add(350, 500, 600)

    # ----------------------------------
    # EMu  (FIXED INDENTATION)
    # ----------------------------------
    elif flav == "EMu":

        if era.startswith("2016"):
            if jet == "LowJet":
                add(175, 250, 325, 425) if is_ltcut else add(250, 400)
            else:
                add(250, 290, 340, 400, 500) if is_ltcut else add(200, 300, 400)

        elif era == "2017":
            if jet == "LowJet":
                add(175, 250, 350, 450) if is_ltcut else add(250, 450)
            else:
                add(250, 300, 350, 400, 500) if is_ltcut else add(250, 350, 500)

        elif era == "2018":
            if jet == "LowJet":
                add(250, 300, 400, 500) if is_ltcut else add(250, 550)
            else:
                add(300, 350, 425, 500, 700) if is_ltcut else add(250, 400, 500)

    # ----------------------------------
    # Final edge
    # ----------------------------------
    edges.append(1200.0)

    return edges
