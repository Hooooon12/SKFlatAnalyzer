import os 


def GetSF(ppass, pfail):
    #print ("pass = " + str(ppass))
    return ( ppass/ (ppass+pfail))


    
if True:
    OS_Data_F = 154.0 
    OS_Data_P =966.0
    
    
    print("Uncorr SF = " + str(GetSF(OS_Data_P,OS_Data_F)))


    for i in range(100):
        SSFake_P = 0.14*i
        SSFake_F= 0.86*i
        
        OS_Data_P_Corr =OS_Data_P - SSFake_P
        OS_Data_F_Corr = OS_Data_F - SSFake_F

    
        print("corr SF [NFake" + str(i)+" ] = " + str(GetSF(OS_Data_P_Corr,OS_Data_F_Corr)))







if False:
    OS_Data_F = 31.0
    OS_Data_P = 139.0
    

    print("Uncorr SF = " + str(GetSF(OS_Data_P,OS_Data_F)))

    
    for i in range(100):
        SSFake_P = 0.1*i
        SSFake_F= 0.9*i
    
        OS_Data_P_Corr =OS_Data_P - SSFake_P
        OS_Data_F_Corr = OS_Data_F - SSFake_F


        print("corr SF [NFake" + str(i)+" ] = " + str(GetSF(OS_Data_P_Corr,OS_Data_F_Corr)))


